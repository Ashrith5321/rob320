#include "mbot_driver/mbot_driver.hpp"

using rix::msg::standard::UInt32;
using rix::ipc::interfaces::Client;
using rix::ipc::interfaces::Connection;

#include <cerrno>
#include <cstring>
#include <vector>
#include <unistd.h>

#include "rix/msg/serialization.hpp"


MBotDriver::MBotDriver(std::unique_ptr<interfaces::Server> server, std::unique_ptr<interfaces::Client> client,
                       std::unique_ptr<MBotBase> mbot)
    : server(std::move(server)), client(std::move(client)), mbot(std::move(mbot)) {
    this->mbot->set_callback(std::bind(&MBotDriver::on_pose_callback, this, std::placeholders::_1));
}

static bool write_all(Connection &con, const uint8_t* src, size_t n){
    size_t i = 0;
    while(i<n){
        size_t j = con.write(src + i, n-i);
        if(j<=0) return false;
        i += static_cast<size_t>(j);
    }
    return true;
}


// static bool read_exact(Client &c, uint8_t *dst, size_t n){
//     size_t rec = 0;
//     while(rec<n){
//         ssize_t r = c.read(dst + rec , n- rec);
//         if(r<=0) return false;
//         rec += static_cast<size_t>(r);
//     }
//     return true;
// }


/**< TODO */
void MBotDriver::spin(std::unique_ptr<interfaces::Notification> notif) {
    mbot->spin();

    std::weak_ptr<interfaces::Connection> accepted;

    while (!notif->is_ready()) {
        if (!server->wait_for_accept(rix::util::Duration{})) {
            continue; 
        }

        if (!server->accept(accepted)) {
            continue; 
        }

        {
            std::lock_guard<std::mutex> lk(connection_mtx);
            connection = accepted;
        }

        auto sp = accepted.lock();
        if (!sp) {
            continue;
        }

        Endpoint peer = sp->remote_endpoint();
        Endpoint goal_ep(peer.address, 8300);

        client->set_nonblocking(true);

        //client->reset();
        client->connect(goal_ep);
        if (!client->wait_for_connect(rix::util::Duration{}) ) {
            break;
        }
    

        if(client->wait_for_readable(rix::util::Duration{})){
            break; 
        }

            uint8_t lenbuf[4];
            if (client->read(lenbuf, 4) <=0) {
                client->reset();
                continue; // 
            }

            UInt32 n{};
            size_t off_len = 0;
            if (!n.deserialize(lenbuf, sizeof(lenbuf), off_len)) {
                //client->reset();
                break;
            }
            const uint32_t body_len = n.data;
            if (body_len == 0) {
                continue; 
            }

            if(client->wait_for_readable(rix::util::Duration{})){
                break; 
            }

            std::vector<uint8_t> body(body_len);
            if (client->read(body.data(), body.size()) <=0) {
                client->reset();
                continue;;
            }

            Pose2DStamped goal{};
            size_t off_body = 0;
            if (!goal.deserialize(body.data(), body.size(), off_body)) {
                //client->reset();
                break;
            }
 
            mbot->drive_to(goal);
        
    }

}

/**< TODO */
void MBotDriver::on_pose_callback(const Pose2DStamped &pose) {
    const size_t payload_size = pose.size();
    std::vector<uint8_t> payload(payload_size);
    size_t off = 0;
    pose.serialize(payload.data(), off);

    UInt32 n;
    n.data = static_cast<uint32_t>(payload.size());
    uint8_t nbuf[4]; 
    size_t noff = 0;
    n.serialize(nbuf, noff); // serialize?? ? 

    std::lock_guard<std::mutex> lk(connection_mtx);
    auto sp = connection.lock(); 
    if (!sp) return;

    if (!write_all(*sp, nbuf, sizeof(nbuf))) return; 
    if (!payload.empty()) {
        (void)write_all(*sp, payload.data(), payload.size());
    }
}