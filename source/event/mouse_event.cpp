#include "mouse_event.hpp"


#include "linux/udev_input_event.hpp"
mouse_event::mouse_event(const udev_input_event & event) {
    // Copies the udev input event
    *this = event;
}

#include "linux/udev_input_event.hpp"
void mouse_event::operator=(const udev_input_event & event) {
    // Determines if this is an "axis" event
    if (event.type == EV_REL) {
        // Converts the event code
        if      (event.code == REL_X) this->code = MOUSE_REL_X;
        else if (event.code == REL_Y) this->code = MOUSE_REL_Y;
        else if (event.code == REL_HWHEEL) this->code = MOUSE_REL_HWHEEL;
        else if (event.code ==  REL_WHEEL) this->code = MOUSE_REL_VWHEEL;
        // Converts the event type
        this->type = MOUSE_EV_REL;

    // Determines if this is a "button" event
    } else if (event.type == EV_KEY) {
        // Converts the event code
        if      (event.code ==   BTN_LEFT) this->code = MOUSE_BTN_L;
        else if (event.code ==  BTN_RIGHT) this->code = MOUSE_BTN_R;
        else if (event.code == BTN_MIDDLE) this->code = MOUSE_BTN_M;
        // Converts the event type
        this->type = MOUSE_EV_BTN;
    }
    // Converts the event value
    this->value = event.value;
}


std::vector<uint8_t> mouse_event::serialize() {
    std::vector<uint8_t> buffer = {
        'm', 'o', 'u', 's', 'e'
    };
    { // Writes the type
    uint8_t* p = (uint8_t*)&this->type;
    buffer.push_back(*p++);
    buffer.push_back(*p++);
    }
    { // Writes the code
    uint8_t* p = (uint8_t*)&this->code;
    buffer.push_back(*p++);
    buffer.push_back(*p++);
    }
    { // Writes the value
    uint8_t* p = (uint8_t*)&this->value;
    buffer.push_back(*p++);
    buffer.push_back(*p++);
    buffer.push_back(*p++);
    buffer.push_back(*p++);
    }
    // Returns the buffer
    return buffer;
}

#include <stdexcept>
void mouse_event::deserialize(const std::vector<uint8_t> & buffer) {
    const uint8_t* b = &buffer[0];
    { // Reads the identifier
    if (*b++ != 'm' || *b++ != 'o' || *b++ != 'u' || *b++ != 's' || *b++ != 'e')
        throw std::runtime_error("mouse_event::deserialize(): not mouse event");
    }
    { // Reads the type
    uint8_t* p = (uint8_t*)&this->type;
    *p++ = *b++;
    *p++ = *b++;
    }
    { // Reads the code
    uint8_t* p = (uint8_t*)&this->code;
    *p++ = *b++;
    *p++ = *b++;
    }
    { // Reads the value
    uint8_t* p = (uint8_t*)&this->value;
    *p++ = *b++;
    *p++ = *b++;
    *p++ = *b++;
    *p++ = *b++;
    }
}

#include <unistd.h>
#include <stdexcept>
#include "linux/udev_input_event.hpp"
mouse_event mouse_event::read(int mouse) {
    udev_input_event udev;
    // This expects udev_input_events. In this case, we don't need a temporary
    // buffer. The stream can be read directly into the udev_input_event
    // object. We can then convert it into a generic mouse_event object.
    if (::read(mouse, &udev, sizeof udev) == -1) {
        throw std::runtime_error("mouse_event::read(): " + std::to_string(errno));
    }
    // Returns the mouse_event
    return { udev };
}

#include <sys/socket.h>
#include <stdexcept>
mouse_event mouse_event::recv(int socket) {
    std::vector<uint8_t> buffer (1024, 0);
    // This expects serialized mouse_events. In this case, we need a temporary
    // buffer. This buffer is passed to mouse_event::deserialize. This should
    // intialize the mouse_event.
    if (::recv(socket, &buffer[0], 1024, 0) == -1) {
        throw std::runtime_error("recv(): " + std::to_string(errno));
    }
    // Deserialize the mouse event
    mouse_event event;
    event.deserialize(buffer);
    // Return the event
    return event;
}