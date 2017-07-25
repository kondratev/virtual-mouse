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
#include "event/mouse_event.hpp"
#include "event/linux/udev_input_event.hpp"
#include "event/linux/vmouse.hpp"

int main (void) {
    int port = 20000;
    int family = AF_INET;
    const char * address = "192.168.1.10";

    int mouse;
    int master_socket;
    struct sockaddr_in address4;
    struct sockaddr_in6 address6;
    socklen_t address4_len;
    socklen_t address6_len;

    // Determines if we can open the mouse node. If this fails, we probably
    // don't have permission to open the mouse node.
    if ((mouse = open(get_vmouse_path().c_str(), O_WRONLY)) == -1) {
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

    while (true) {
        mouse_event event = mouse_event::recv(master_socket);
        udev_input_event input { event };
        std::cout << (int)event.type << " " << (int)event.code << " " << event.value << std::endl;
        input.write(mouse);
    }
}
