#ifndef NETWORK
#define NETWORK

#include <string>
std::string inline sockaddr_get_address(int family, struct sockaddr address, socklen_t length) {
    // Stores the address in a character buffer. This should be more than
    // enough characters to store either an ipv4 or ipv6 address. It is
    // initialized to zero to prevent overflows.
    char paddress [1024] = { 0 };

    if (family == AF_INET) {
        // Retrieves an ipv4 address
        inet_ntop(AF_INET, &((struct sockaddr_in*)&address)->sin_addr, paddress, length);
    }
    if (family == AF_INET6) {
        // Retrieves an ipv6 address
        inet_ntop(AF_INET6, &((struct sockaddr_in6*)&address)->sin6_addr, paddress, length);
    } 
    
    // Returns the address
    return { paddress };
}

#endif // NETWORK