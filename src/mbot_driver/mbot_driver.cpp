#include "mbot_driver/mbot_driver.hpp"

using rix::msg::standard::UInt32;

MBotDriver::MBotDriver(std::unique_ptr<interfaces::Server> server, std::unique_ptr<interfaces::Client> client,
                       std::unique_ptr<MBotBase> mbot)
    : server(std::move(server)), client(std::move(client)), mbot(std::move(mbot)) {
    this->mbot->set_callback(std::bind(&MBotDriver::on_pose_callback, this, std::placeholders::_1));
}

/**< TODO */
void MBotDriver::spin(std::unique_ptr<interfaces::Notification> notif) {}

/**< TODO */
void MBotDriver::on_pose_callback(const Pose2DStamped &pose) {}