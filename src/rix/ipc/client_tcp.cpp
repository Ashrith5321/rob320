#include "rix/ipc/client_tcp.hpp"

namespace rix {
namespace ipc {

/**< TODO */
ClientTCP::ClientTCP() : socket(AF_INET, SOCK_STREAM){}

ClientTCP::ClientTCP(const ClientTCP &other) : socket(other.socket) {}

ClientTCP &ClientTCP::operator=(const ClientTCP &other) {
    if (this != &other) {
        socket = other.socket;
    }
    return *this;
}

ClientTCP::~ClientTCP() {}

/**< TODO *///-------------------------------
bool ClientTCP::connect(const Endpoint &endpoint) { 
    int flags = ::fcntl(socket.fd(), F_GETFL, 0);

    if (flags == -1) {
        reset();
        flags = ::fcntl(socket.fd(), F_GETFL, 0);
    }

    bool nb = (flags & O_NONBLOCK) != 0;
    bool ok = socket.connect(endpoint);

    if (nb) return false;        
    return ok;   
 }


ssize_t ClientTCP::write(const uint8_t *buffer, size_t len) const { 
    return ::send(socket.fd(), buffer, len, 0);
 }


ssize_t ClientTCP::read(uint8_t *buffer, size_t len) const { 
    return ::recv(socket.fd(), buffer, len, 0);
 }


Endpoint ClientTCP::remote_endpoint() const { 
    Endpoint e;
    socket.getpeername(e);

    return e; 
 }

Endpoint ClientTCP::local_endpoint() const { 
    Endpoint e;
    socket.getsockname(e);

    return e; 
 }

bool ClientTCP::ok() const { return socket.ok(); }

/**< TODO */ //--------------------------------------------------------------
bool ClientTCP::wait_for_connect(const rix::util::Duration &duration) const { 
    if (!wait_for_writable(duration)) return false;

    int soerr = 0;
    socklen_t len = sizeof(soerr);
    if (::getsockopt(socket.fd(), SOL_SOCKET, SO_ERROR, &soerr, &len) < 0) {
        return false;
    }
    return soerr == 0;
 }


bool ClientTCP::wait_for_writable(const rix::util::Duration &duration) const { 
    struct pollfd fds;
    fds.fd = this->socket.fd();
    fds.events = POLLOUT;
    fds.revents = 0;
    int num = ::poll(&fds, 1, static_cast<int>(duration.to_milliseconds()));
    if(num > 0 && (fds.revents & POLLOUT)) return true;
    else return false;
 }


bool ClientTCP::wait_for_readable(const rix::util::Duration &duration) const { 
    struct pollfd fds;
    fds.fd = this->socket.fd();
    fds.events = POLLIN;
    fds.revents = 0;
    int num = ::poll(&fds, 1, static_cast<int>(duration.to_milliseconds()));
    if(num > 0 && (fds.revents & POLLIN)) return true;
    else return false;
 }

void ClientTCP::set_nonblocking(bool status) {

    int flag = fcntl(this->socket.fd(), F_GETFL);
    if(flag == -1){
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

bool ClientTCP::is_nonblocking() const {
    int flag = fcntl(this->socket.fd(), F_GETFL);

    if(flag & O_NONBLOCK) return true;
    return false;
 
}

void ClientTCP::reset() { socket = Socket(AF_INET, SOCK_STREAM); }

}  // namespace ipc
}  // namespace rix