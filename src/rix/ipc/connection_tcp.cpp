#include "rix/ipc/connection_tcp.hpp"

namespace rix {
namespace ipc {

ConnectionTCP::ConnectionTCP(const Socket &socket) : socket(socket) {}

ConnectionTCP::ConnectionTCP() : socket() {}

ConnectionTCP::ConnectionTCP(const ConnectionTCP &other) : socket(other.socket) {}

ConnectionTCP &ConnectionTCP::operator=(const ConnectionTCP &other) {
    if (this != &other) {
        socket = other.socket;
    }
    return *this;
}

ConnectionTCP::~ConnectionTCP() {}

/**< TODO */
ssize_t ConnectionTCP::write(const uint8_t *buffer, size_t len) const { return -1; }

/**< TODO */
ssize_t ConnectionTCP::read(uint8_t *buffer, size_t len) const { return -1; }

/**< TODO */
Endpoint ConnectionTCP::remote_endpoint() const { return {}; }

/**< TODO */
Endpoint ConnectionTCP::local_endpoint() const { return {}; }

/**< TODO */
bool ConnectionTCP::ok() const { return false; }

/**< TODO */
bool ConnectionTCP::wait_for_writable(const rix::util::Duration &duration) const { return false; }

/**< TODO */
bool ConnectionTCP::wait_for_readable(const rix::util::Duration &duration) const { return false; }

/**< TODO */
void ConnectionTCP::set_nonblocking(bool status) {}

/**< TODO */
bool ConnectionTCP::is_nonblocking() const { return false; }

}  // namespace ipc
}  // namespace rix