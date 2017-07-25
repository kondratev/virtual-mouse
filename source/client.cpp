// C includes
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
// C++ includes
#include <iostream>
#include <stdexcept>
// Includes
#include "mouse_event.hpp"
#include "linux/mouse.hpp"

int main (void) {
    int port = 20000;
    int family = AF_INET;
    const char * address = "192.168.1.10";

    int vmouse;
    int master_socket;
    struct sockaddr_in address4;
    struct sockaddr_in6 address6;
    socklen_t address4_len;
    socklen_t address6_len;

    std::cout << get_vmouse_path() << std::endl;

    // Determines if we can open the mouse node. If this fails, we probably
    // don't have permission to open the mouse node.
    if ((vmouse = open(get_vmouse_path().c_str(), O_WRONLY)) == -1) {
        throw std::runtime_error("open(): " + std::to_string(errno));
    }

    // Determines if we can create a master socket. This should succeed on 99%
    // of machines. Either AF_INET (ip protocol 4) or AF_INET6 (ip protocol 6)
    // supports SOCK_DGRAM.
    if ((master_socket = socket(family, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        throw std::runtime_error("socket(): " + std::to_string(errno));
    }

    // Assigns the address family
    address4.sin_family = AF_INET;
    address6.sin6_family = AF_INET6;
    // Assigns the address port
    address4.sin_port = htons(port);
    address6.sin6_port = htons(port);
    // Assigns the address string
    inet_pton(AF_INET, address, &address4.sin_addr);
    inet_pton(AF_INET6, address, &address6.sin6_addr);
    // Assigns the address length
    address4_len = sizeof(address4);
    address6_len = sizeof(address6);


    const char * connect = "connect";
    sendto(master_socket, connect, strlen(connect), 0, (struct sockaddr*)&address4, address4_len);

    char message [MOUSE_EVENT_SLEN];
    while (true) {
        if (family == AF_INET) {
            if (recv(master_socket, &message, MOUSE_EVENT_SLEN, 0) == -1) {
                throw std::runtime_error("recv(): " + std::to_string(errno));
            }
	    
	    mouse_event event = mouse_event_deserialize(message);
            input_event input = mouse_event_to_input_event(event);
	    std::cout << input.type << std::endl;
	    std::cout << input.code << std::endl;
	    std::cout << input.value << std::endl;
	    std::cout << std::endl;
	    write(vmouse, &input, sizeof(input_event));
        }
    }
}
