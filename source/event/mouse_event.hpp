#ifndef MOUSE_EVENT
#define MOUSE_EVENT

#define MOUSE_EV_REL 1
#define MOUSE_EV_BTN 2

#define MOUSE_REL_X 1
#define MOUSE_REL_Y 2
#define MOUSE_REL_HWHEEL 3
#define MOUSE_REL_VWHEEL 4

#define MOUSE_BTN_L 1
#define MOUSE_BTN_R 2
#define MOUSE_BTN_M 3

struct udev_input_event;

#include <stdint.h>
#include <string>
struct mouse_event {
    uint16_t type { 0 };
    uint16_t code { 0 };
    int32_t value { 0 };

    mouse_event() = default;
    static mouse_event recv(int);

    #ifdef __unix__
    mouse_event(const udev_input_event&);
    void operator=(const udev_input_event&);
    std::string serialize();
    void deserialize(const std::string &);
    static mouse_event read(int);
    #endif // __unix__
};

#endif // MOUSE_EVENT