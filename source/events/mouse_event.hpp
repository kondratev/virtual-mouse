#ifndef MOUSE_EVENT
#define MOUSE_EVENT

// Defines the "generic" mouse event identifiers
#define MOUSE_NONE 0 // no mouse (event, axis, button)
#define MOUSE_EV_REL 1 // axis (x, y, vwheel, hwheel)
#define MOUSE_EV_BTN 2 // buttons (left, right, middle)
#define MOUSE_EV_SYN 3 // synchonize
#define MOUSE_REL_X 1 // x axis
#define MOUSE_REL_Y 2 // y axis
#define MOUSE_REL_HWHEEL 3 // hwheel axis
#define MOUSE_REL_VWHEEL 4 // vwheel axis
#define MOUSE_BTN_L 1 // left mouse button
#define MOUSE_BTN_R 2 // right mouse button
#define MOUSE_BTN_M 3 // middle mouse button

#include <stdint.h>
#include <string>
#include <vector>

// If we are on *nix platforms, we will need to process udev events. Other
// operating systems use their own event handlers. 
#ifdef __unix__
struct udev_input_event;
#endif

// This is used as a generic mouse event. This acts as a translator for
// *nix->windows events and windows->*nix events. It also allows those events
// to be serialized and sent over the network. Currently, this only supports
// basic mouse events.
struct mouse_event {
    uint16_t type { MOUSE_NONE };
    uint16_t code { MOUSE_NONE };
    int32_t value { MOUSE_NONE };

    // Both Windows/*nix define custom copy constructors. This behavior
    // automatically disables the default constructor. This will enables
    // the default constructor.
    mouse_event() = default;

    // Recieves mouse event from the network stream. This is non-blocking
    // because blocking would prevent the application from terminating in
    // a timely manner.
    static mouse_event recv(int);

    // Serializes/deserializes the mouse_event. This converts any values
    // into/from network-byte-order before sending/recieving data frin
    // the network stream.
    std::vector<uint8_t> serialize();
    void deserialize(const std::vector<uint8_t>&);

    #ifdef __unix__
        // Defines the copy constructor and operator for a udev_input_event. This
        // allows the udev_input_event to be copied directly into a mouse_event.
        // This is convenient when reading input_events from udev.
        mouse_event(const udev_input_event&);
        void operator=(const udev_input_event&);

        // Reads input_events from udev
        static mouse_event read(int);
    #endif // __unix__
};

#endif // MOUSE_EVENT