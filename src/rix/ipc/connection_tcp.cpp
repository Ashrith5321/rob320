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
ssize_t ConnectionTCP::write(const uint8_t *buffer, size_t len) const {
    return ::send(socket.fd(), buffer, len, 0);}


/**< TODO */
ssize_t ConnectionTCP::read(uint8_t *buffer, size_t len) const {
    return ::recv(socket.fd(), buffer, len, 0);
}

/**< TODO */
Endpoint ConnectionTCP::remote_endpoint() const {
    Endpoint e;
    socket.getpeername(e);


     return e; 
    }

/**< TODO */
Endpoint ConnectionTCP::local_endpoint() const { 
        Endpoint e;
    socket.getsockname(e);


     return e; 
 }

/**< TODO */
bool ConnectionTCP::ok() const { return socket.ok(); }

/**< TODO */
bool ConnectionTCP::wait_for_writable(const rix::util::Duration &duration) const { 
    struct pollfd fds;
    fds.fd = this->socket.fd();
    fds.events = POLLOUT;
    fds.revents = 0;
    int num = ::poll(&fds, 1, static_cast<int>(duration.to_milliseconds()));
    if(num > 0 && (fds.revents & POLLOUT)) return true;
    else return false;

 }

/**< TODO */
bool ConnectionTCP::wait_for_readable(const rix::util::Duration &duration) const { 
    struct pollfd fds;
    fds.fd = this->socket.fd();
    fds.events = POLLIN;
    fds.revents = 0;
    int num = ::poll(&fds, 1, static_cast<int>(duration.to_milliseconds()));
    if(num > 0 && (fds.revents & POLLIN)) return true;
    else return false;
 }



/**< TODO */
void ConnectionTCP::set_nonblocking(bool status) {
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
bool ConnectionTCP::is_nonblocking() const { 
    
    int flag = fcntl(this->socket.fd(), F_GETFL);

    if(flag & O_NONBLOCK) return true;
    return false;
}
}  // namespace ipc
}  // namespace rix



