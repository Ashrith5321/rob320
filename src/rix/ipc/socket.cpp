#include "rix/ipc/socket.hpp"

namespace rix {
namespace ipc {

Socket::Socket() : File() {}

Socket::Socket(int domain, int type)
    : File(socket(domain, type, 0)), domain(domain), type(type), _bound(false), _listening(false) {}

Socket::Socket(int fd, int domain, int type) : File(fd), domain(domain), type(type), _bound(false), _listening(false) {}

Socket::Socket(const Socket &src)
    : File(src), domain(src.domain), type(src.type), _bound(src._bound), _listening(src._listening) {}

Socket &Socket::operator=(const Socket &src) {
    if (this != &src) {
        (File &)*this = src;
        domain = src.domain;
        type = src.type;
        _bound = src._bound;
        _listening = src._listening;
    }
    return *this;
}

Socket::Socket(Socket &&src)
    : File(std::move(src)),
      domain(std::move(src.domain)),
      type(std::move(src.type)),
      _bound(std::move(src._bound)),
      _listening(std::move(src._listening)) {}

Socket &Socket::operator=(Socket &&src) {
    (File &)*this = std::move(src);
    domain = std::move(src.domain);
    type = std::move(src.type);
    _bound = std::move(src._bound);
    _listening = std::move(src._listening);
    return *this;
}

Socket::~Socket() {}

/**< TODO
 * Hint: You only need to consider the case when domain is AF_INET (IPv4)
 */
bool Socket::bind(const Endpoint &endpoint) { 
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(endpoint.port);
    inet_pton(AF_INET, endpoint.address.c_str(), &addr.sin_addr);
    
    socklen_t addrlen = sizeof(addr);

    int num = ::bind( fd(), (struct sockaddr*) &addr,  addrlen);
    if(num == 0){
        _bound = true;
        return true;
    }    
    return false;
}

/**< TODO */
bool Socket::listen(int backlog) { 
    int num = ::listen(fd(), backlog);
    if(num == 0){
        _listening = true;
        return true;
    }
    return false; }

/**< TODO
 * Hint: You only need to consider the case when domain is AF_INET (IPv4)
 */
bool Socket::connect(const Endpoint &endpoint) { 
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    
    int num = ::connect(fd(), (struct sockaddr*) &addr, addrlen);
    if(num == 0) return true;
    return false; }

/**< TODO */
bool Socket::accept(Socket &sock) { 
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int num = ::accept(fd(), (struct sockaddr*) &addr, &addrlen);
    if(num != -1){
        sock = Socket(num, domain, type);
        return true;
    }
    return false;
    
 }

/**< TODO
 * Hint: You only need to consider the case when domain is AF_INET (IPv4)
 */
bool Socket::getsockname(Endpoint &endpoint) const { 
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int num = ::getsockname(fd(), (struct sockaddr*) &addr, &addrlen);
    if(num != -1){
        char addr_str[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &addr.sin_addr, addr_str, sizeof(addr_str))) {
            endpoint.address = addr_str;
            endpoint.port = ntohs(addr.sin_port);
            return true;
        }
    }
    return false;
 }

/**< TODO
 * Hint: You only need to consider the case when domain is AF_INET (IPv4)
 */
bool Socket::getpeername(Endpoint &endpoint) const {
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int num = ::getpeername(fd(), (struct sockaddr*) &addr, &addrlen);
    if(num != -1){
        char addr_str[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &addr.sin_addr, addr_str, sizeof(addr_str))) {
            endpoint.address = addr_str;
            endpoint.port = ntohs(addr.sin_port);
            return true;
        }
    }
    return false;
}

/**< TODO */
bool Socket::getsockopt(int level, int optname, int &value) { 
    int num = ::getsockopt(fd(), level, optname, &value, (socklen_t *) sizeof(value));
    if(num == 0) return true;
    return false; }

/**< TODO */
bool Socket::setsockopt(int level, int optname, int value) { 
    int num = ::setsockopt(fd(), level, optname, &value, (socklen_t) sizeof(value));
    if(num == 0) return true;   
    return false; }

bool Socket::is_bound() const { return _bound; }

bool Socket::is_listening() const { return _listening; }

}  // namespace ipc
}  // namespace rix