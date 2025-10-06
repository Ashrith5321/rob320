#include "lidar_driver/lidar_driver.hpp"
using rix::msg::standard::UInt32;

using rix::ipc::interfaces::Connection;

LidarDriver::LidarDriver(std::unique_ptr<interfaces::Server> server, std::unique_ptr<LidarBase> lidar)
    : server(std::move(server)), lidar(std::move(lidar)) {
    this->lidar->set_callback(std::bind(&LidarDriver::on_scan_callback, this, std::placeholders::_1));
}

LidarDriver::~LidarDriver() { lidar->set_callback(nullptr); }

static bool write_all(Connection &con, const uint8_t* src, size_t n){
    size_t i = 0;
    while(i<n){
        size_t j = con.write(src + i, n-i);
        if(j<=0) return false;
        i += static_cast<size_t>(j);
    }
    return true;
}

/**< TODO */
void LidarDriver::spin(std::unique_ptr<interfaces::Notification> notif) {
    lidar->spin();

    std::weak_ptr<interfaces::Connection> accepted;

    while (!notif->is_ready()) {
 
        if ( !server->wait_for_accept( rix::util::Duration{})){
            continue;
        }


        if (server->accept(accepted)){
            std::lock_guard< std::mutex > lock(connection_mtx);
            connection = accepted;
        }
    }
}

/**< TODO */
void LidarDriver::on_scan_callback(const rix::msg::sensor::LaserScan &scan) {
    
    std::vector<uint8_t> payload(scan.size());
    size_t off = 0;
    scan.serialize(payload.data(),off);

    UInt32 n;
    n.data = payload.size();
    uint8_t nbuf[4];
    size_t noff = 0;
    n.serialize(nbuf, noff);

    // send it
    std::lock_guard<std::mutex> lk(connection_mtx);
    auto sp = connection.lock();
    if (sp) {
        if (!write_all(*sp, nbuf,4)) return;
        if (!payload.empty() ) write_all(*sp, payload.data(), payload.size());
    }
}
