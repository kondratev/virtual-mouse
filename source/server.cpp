// C includes
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
// C++ includes
#include <iostream>
#include <stdexcept>
#include <vector>
#include <thread>
#include <mutex>
// Includes
#include "event/mouse_event.hpp"
#include "event/linux/udev_input_event.hpp"

bool running;
std::mutex clients_mu;

void recv_client(int master_socket, std::vector<struct sockaddr> & clients) {
    // When a client connects to the server, we need the client connection
    // information. This is so the server can send messages back to the client.
    // TCP does not require this extra step, but UDP does.
    struct sockaddr caddress;
    socklen_t caddress_len = sizeof(caddress);

    while (running) {
        // Retrieves a message from a client. Currently, clients only send out
        // "connect" message. This is interpreted as connecting the client to
        // the server. Eventually "disconnect" and "keepalive" messages will
        // ensure client integrity.
        char message [1024] = { 0 };
        // Recieves a message from the client
        if (recvfrom(master_socket, &message, 1024, 0, &caddress, &caddress_len) != -1) {
            if (!strcmp(message, "connect")) {
                // It is possible that updates are being sent to the clients.
                // In this case, we will wait until all updates have been sent.
                std::lock_guard<std::mutex> lock (clients_mu);
                clients.push_back(caddress);
            }
        }
    }
}

void send_mouse(int master_socket, int mouse, std::vector<struct sockaddr> & clients) {
    while (running) {
        // Retrieves a mouse event. This is "blocking;" meaning that this
        // thread will stall until an event is recieved. This prevents the
        // thread from hogging cpu cycles.
        mouse_event event = mouse_event::read(mouse);
        std::cout << (int)event.type << " " << (int)event.code << " " << event.value << std::endl;
        // Determines if the event is valid
        if (event.type == MOUSE_EV_REL ||
            event.type == MOUSE_EV_BTN) {
            std::vector<uint8_t> data = event.serialize();
            std::lock_guard<std::mutex> lock (clients_mu);
            for (auto & client : clients) {
                sendto(master_socket, &data[0], data.size(), 0, &client, sizeof(client));
            }
        }
    }
}

int main (void) {
    int port = 20000;
    int family = AF_INET;
    const char * address = "192.168.1.10";
    const char * mouse_node = "/dev/input/event0";

    int mouse;
    int master_socket;
    struct sockaddr_in address4;
    struct sockaddr_in6 address6;
    socklen_t address4_len;
    socklen_t address6_len;
    std::vector<struct sockaddr> clients;

    // Determines if we can open the mouse node. If this fails, we probably
    // don't have permission to open the mouse node.
    if ((mouse = open(mouse_node, O_RDONLY)) == -1) {
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

    // Determines if we can bind to a specific address. If this fails, then
    // clients cannot predict where to connect to the server. This will fail
    // if either the address or port is invalid.
    if (family == AF_INET) {
        // Determines if the socket could bind
        if (bind(master_socket, (struct sockaddr*)&address4, address4_len) == -1)
            throw std::runtime_error("bind(): " + std::to_string(errno));
    } else if (family == AF_INET6) {
        // Determines if the socket could bind
        if (bind(master_socket, (struct sockaddr*)&address6, address6_len) == -1)
            throw std::runtime_error("bind(): " + std::to_string(errno));
    }

    // Starts the program
    running = true;
    // Starts processing clients and mouse updates
    std::thread t0 { recv_client, master_socket, std::ref(clients) };
    std::thread t1 { send_mouse, master_socket, mouse, std::ref(clients) };
    // Prevents the program from terminating
    std::cin.get();
    // Stops the program
    running = false;
    // Joins the two threads
    t0.join();
    t1.join();

    return 0;
}
