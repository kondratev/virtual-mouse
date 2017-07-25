#ifdef __unix__

#include "udev_input_event.hpp"

int32_t get_current_usec() {
    // When processing events on Linux, the current time in microseconds is
    // required. Instead of transmitting the current time across the network,
    // we'll retrieve it on the client machine. 
    static struct timeval time;
    gettimeofday(&time, nullptr);
    // Returns the current time in microseconds
    return time.tv_usec;
}

#include "event/mouse_event.hpp"
void udev_input_event::operator=(const mouse_event & event) {
    // Retrieves the current time
    this->time.tv_usec = get_current_usec();
    // Determines if this is an "axis" event
    if (event.type == MOUSE_EV_REL) {
        // Converts the event code
        if      (event.code == MOUSE_REL_X) this->code = REL_X;
        else if (event.code == MOUSE_REL_Y) this->code = REL_Y;
        else if (event.code == MOUSE_REL_HWHEEL) this->code = REL_HWHEEL;
        else if (event.code == MOUSE_REL_VWHEEL) this->code = REL_WHEEL;
        // Converts the event type
        this->type = EV_REL;

    // Determines if this is a "button" event
    } else if (event.type == MOUSE_EV_BTN) {
        // Converts the event code
        if      (event.code == MOUSE_BTN_L) this->code = BTN_LEFT;
        else if (event.code == MOUSE_BTN_R) this->code = BTN_RIGHT;
        else if (event.code == MOUSE_BTN_M) this->code = BTN_MIDDLE;
        // Converts the event type
        this->type = EV_KEY;
    }
    // Converts the event value
    this->value = event.value;
}

#endif // __unix__