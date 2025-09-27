#include "rix/ipc/server_tcp.hpp"

namespace rix {
namespace ipc {

/**< TODO */
ServerTCP::ServerTCP(const Endpoint &ep, size_t backlog) {}

ServerTCP::ServerTCP() : socket(), connections() {}

ServerTCP::ServerTCP(const ServerTCP &other) : socket(other.socket), connections(other.connections) {}

ServerTCP &ServerTCP::operator=(const ServerTCP &other) {
    if (this != &other) {
        socket = other.socket;
        connections = other.connections;
    }
    return *this;
}

ServerTCP::~ServerTCP() {}

/**< TODO */
void ServerTCP::close(const std::weak_ptr<interfaces::Connection> &connection) {}

/**< TODO */
bool ServerTCP::ok() const { return false; }

/**< TODO */
Endpoint ServerTCP::local_endpoint() const { return {}; }

/**< TODO */
bool ServerTCP::accept(std::weak_ptr<interfaces::Connection> &connection) { return false; }

/**< TODO */
bool ServerTCP::wait_for_accept(rix::util::Duration duration) const { return false; }

/**< TODO */
void ServerTCP::set_nonblocking(bool status) {}

/**< TODO */
bool ServerTCP::is_nonblocking() const { return false; }

}  // namespace ipc
}  // namespace rix