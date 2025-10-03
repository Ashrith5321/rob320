#include "rix/ipc/server_tcp.hpp"
#include <memory>

namespace rix {
namespace ipc {

/**< TODO */
ServerTCP::ServerTCP(const Endpoint &ep, size_t backlog) {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    socket = Socket(AF_INET, SOCK_STREAM);
    int fd = socket.fd();

    socket.bind(ep);
    socket.listen(backlog);
}

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

/**< TODO */ //////////------------------------------------------------------------------------------------------------
void ServerTCP::close(const std::weak_ptr<interfaces::Connection> &connection) {
    if (auto p = connection.lock()) {
        connections.erase(p);
    }
    
}

/**< TODO */
bool ServerTCP::ok() const { return socket.ok(); }

/**< TODO */
Endpoint ServerTCP::local_endpoint() const { 
    Endpoint e;
    socket.getsockname(e);


     return e; 
}

/**< TODO *///////////------------------------------------------------------------------------------------------------
bool ServerTCP::accept(std::weak_ptr<interfaces::Connection> &connection) { 
    Socket new_sock;
    if (!socket.accept(new_sock)) {
        connection.reset();
        return false;
    }

    // Construct directly here (friend context), then upcast to interfaces::Connection
    std::shared_ptr<interfaces::Connection> conn{ new ConnectionTCP(new_sock) };

    connections.insert(conn);
    connection = conn;
    return true;
 }

/**< TODO *///////////------------------------------------------------------------------------------------------------
bool ServerTCP::wait_for_accept(rix::util::Duration duration) const { 
        int fd = socket.fd();
    if (fd < 0) return false;

    struct pollfd pfd{};
    pfd.fd = fd;
    // Listening socket becomes readable when a connection is pending.
    pfd.events = POLLIN;

    int timeout = static_cast<int>(duration.to_milliseconds());

    for (;;) {
        int n = ::poll(&pfd, 1, timeout);
        if (n > 0) {
            if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) return false;
            return (pfd.revents & POLLIN) != 0;
        }
        if (n == 0) return false; // timeout
        if (errno == EINTR) continue;
        return false; // other error
    }
 }

/**< TODO */
void ServerTCP::set_nonblocking(bool status) {
    int flag = fcntl(this->socket.fd(), F_GETFL);
    if(flag == -1){
        //exit(1);
        return;
    }
    if(status){
        flag |= O_NONBLOCK; // no idea what i did
    }
    else{
        flag &= ~O_NONBLOCK; // no idea what i did
    }
    fcntl(socket.fd(), F_SETFL, flag);
}

/**< TODO */
bool ServerTCP::is_nonblocking() const { 
    int flag = fcntl(this->socket.fd(), F_GETFL);

    if(flag & O_NONBLOCK) return true;
    return false;
 }



}  // namespace ipc
}  // namespace rix