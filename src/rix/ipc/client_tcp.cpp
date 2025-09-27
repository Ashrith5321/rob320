#include "rix/ipc/client_tcp.hpp"

namespace rix {
namespace ipc {

/**< TODO */
ClientTCP::ClientTCP() {}

ClientTCP::ClientTCP(const ClientTCP &other) : socket(other.socket) {}

ClientTCP &ClientTCP::operator=(const ClientTCP &other) {
    if (this != &other) {
        socket = other.socket;
    }
    return *this;
}

ClientTCP::~ClientTCP() {}

/**< TODO */
bool ClientTCP::connect(const Endpoint &endpoint) { return false; }

/**< TODO */
ssize_t ClientTCP::write(const uint8_t *buffer, size_t len) const { return -1; }

/**< TODO */
ssize_t ClientTCP::read(uint8_t *buffer, size_t len) const { return -1; }

/**< TODO */
Endpoint ClientTCP::remote_endpoint() const { return {}; }

/**< TODO */
Endpoint ClientTCP::local_endpoint() const { return {}; }

/**< TODO */
bool ClientTCP::ok() const { return false; }

/**< TODO */
bool ClientTCP::wait_for_connect(const rix::util::Duration &duration) const { return false; }

/**< TODO */
bool ClientTCP::wait_for_writable(const rix::util::Duration &duration) const { return false; }

/**< TODO */
bool ClientTCP::wait_for_readable(const rix::util::Duration &duration) const { return false; }

/**< TODO */
void ClientTCP::set_nonblocking(bool status) {}

/**< TODO */
bool ClientTCP::is_nonblocking() const { return false; }

void ClientTCP::reset() { socket = Socket(AF_INET, SOCK_STREAM); }

}  // namespace ipc
}  // namespace rix