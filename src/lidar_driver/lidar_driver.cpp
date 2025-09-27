#include "lidar_driver/lidar_driver.hpp"

LidarDriver::LidarDriver(std::unique_ptr<interfaces::Server> server, std::unique_ptr<LidarBase> lidar)
    : server(std::move(server)), lidar(std::move(lidar)) {
    this->lidar->set_callback(std::bind(&LidarDriver::on_scan_callback, this, std::placeholders::_1));
}

LidarDriver::~LidarDriver() { lidar->set_callback(nullptr); }

/**< TODO */
void LidarDriver::spin(std::unique_ptr<interfaces::Notification> notif) {}

/**< TODO */
void LidarDriver::on_scan_callback(const rix::msg::sensor::LaserScan &scan) {}
