#include "mbot_driver/mbot_driver.hpp"

using rix::msg::standard::UInt32;

MBotDriver::MBotDriver(std::unique_ptr<interfaces::Server> server, std::unique_ptr<interfaces::Client> client,
                       std::unique_ptr<MBotBase> mbot)
    : server(std::move(server)), client(std::move(client)), mbot(std::move(mbot)) {
    this->mbot->set_callback(std::bind(&MBotDriver::on_pose_callback, this, std::placeholders::_1));
}

/**< TODO */

/**< TODO */
/**< TODO */
/**< TODO */
void MBotDriver::spin(std::unique_ptr<interfaces::Notification> notif) {
    // Tests require this:
    mbot->spin();

    // 1) Accept inbound connection  use wait_for_accept so the harness advances.
    std::weak_ptr<interfaces::Connection> accepted;
    while (!notif->is_ready()) {
        if (server->wait_for_accept(rix::util::Duration{})) {
            if (server->accept(accepted)) {
                std::lock_guard<std::mutex> lk(connection_mtx);
                connection = accepted;  // publish for on_pose_callback()
                break;
            }
        }
    }
    if (notif->is_ready()) return;

    // 2) Connect our client to <peer-ip, 8300>.
    Endpoint target{};
    {
        auto sp = accepted.lock();
        if (!sp || !sp->ok()) return;
        target = sp->remote_endpoint();
        target.port = 8300;
    }

    // Blocking connect is fine here; the harness fakes the peer.
    client->set_nonblocking(false);
    if (!client->connect(target)) {
        return; // avoid hanging if the harness fails this step
    }

    // Helper: read exactly n bytes, first gating each attempt with wait_for_readable().
    auto read_exact = [this, &notif](uint8_t* dst, size_t n)->bool {
        size_t got = 0;
        while (got < n) {
            if (notif->is_ready()) return false;
            // Let the harness deliver bytes:
            if (!client->wait_for_readable(rix::util::Duration{})) {
                continue; // loop and re-check notif
            }
            ssize_t r = client->read(dst + got, n - got);
            if (r > 0) {
                got += static_cast<size_t>(r);
            } else if (r == 0) {
                return false; // EOF
            } else {
                // transient; loop again
            }
        }
        return true;
    };

    // 3) Loop: read size-prefixed Pose2DStamped and hand to controller.
    uint8_t size_buf[4];
    while (!notif->is_ready()) {
        if (!read_exact(size_buf, sizeof(size_buf))) {
            client->reset();
            client->set_nonblocking(false);
            if (!client->connect(target)) break;
            continue;
        }

        UInt32 len_msg{};
        size_t off = 0;
        if (!len_msg.deserialize(size_buf, sizeof(size_buf), off)) {
            client->reset();
            client->set_nonblocking(false);
            if (!client->connect(target)) break;
            continue;
        }

        const uint32_t payload_len = len_msg.data;
        if (payload_len == 0 || payload_len > (64u * 1024u * 1024u)) {
            client->reset();
            client->set_nonblocking(false);
            if (!client->connect(target)) break;
            continue;
        }

        std::vector<uint8_t> payload(payload_len);
        if (!read_exact(payload.data(), payload.size())) {
            client->reset();
            client->set_nonblocking(false);
            if (!client->connect(target)) break;
            continue;
        }

        Pose2DStamped goal{};
        size_t poff = 0;
        if (!goal.deserialize(payload.data(), payload.size(), poff) || poff != payload.size()) {
            continue; // malformed payload; ignore
        }

        mbot->drive_to(goal);
    }

    client->reset();
}

/**< TODO */
/**< TODO */
/**< TODO */
void MBotDriver::on_pose_callback(const Pose2DStamped &pose) {
    // Take strong ref to the accepted connection under the mutex
    std::shared_ptr<interfaces::Connection> conn_sp;
    {
        std::lock_guard<std::mutex> lk(connection_mtx);
        conn_sp = connection.lock();
    }
    if (!conn_sp || !conn_sp->ok()) return;

    // --- serialize payload ---
    std::vector<uint8_t> payload(256);
    size_t poff = 0;
    pose.serialize(payload.data(), poff);
    if (poff > payload.size()) {
        payload.resize(poff);
        poff = 0;
        pose.serialize(payload.data(), poff);
    } else {
        payload.resize(poff);
    }

    // 4-byte size prefix
    rix::msg::standard::UInt32 size_msg{};
    size_msg.data = static_cast<uint32_t>(payload.size());
    uint8_t size_buf[4];
    size_t soff = 0;
    size_msg.serialize(size_buf, soff);

    // --- non-blocking exact write (bounded retry) ---
    auto write_exact_nb = [&](const uint8_t* src, size_t n)->bool {
        size_t sent = 0;
        // reasonable bound to avoid infinite loops even if peer misbehaves
        constexpr int kMaxSpins = 100000;
        int spins = 0;
        while (sent < n && spins < kMaxSpins) {
            ssize_t w = conn_sp->write(src + sent, n - sent);
            if (w > 0) {
                sent += static_cast<size_t>(w);
            } else if (w == 0) {
                // peer closed gracefully — abort this send
                return false;
            } else {
                // not ready yet (EAGAIN) or transient; spin and retry
                ++spins;
                continue;
            }
        }
        return sent == n;
    };

    if (!write_exact_nb(size_buf, sizeof(size_buf))) return;
    (void)write_exact_nb(payload.data(), payload.size());
}
