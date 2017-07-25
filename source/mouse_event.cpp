#include "mouse_event.hpp"

mouse_event new_mouse_event(unsigned char type, unsigned char code, long value) {
    struct mouse_event event;
    event.type = type;
    event.code = code;
    event.value = value;
    return event;
}

#include <arpa/inet.h>
#include <string.h>
const char * mouse_event_serialize(const mouse_event & event) {
    // Stores the mouse event buffer
    static char buffer [MOUSE_EVENT_SLEN] = { 0 };

    char * p = &buffer[0];
    // Writes mouse identifier to message
    *p++ = 'm';
    *p++ = 'o';
    *p++ = 'u';
    *p++ = 's';
    *p++ = 'e';
    // Writes event type to message
    *p++ = event.type;
    // Writes event code to message
    *p++ = event.code;
    // Writes event value to message
    memcpy(p, &event.value, sizeof event.value);
    // Returns the buffer
    return buffer;
}

#include <stdexcept>
mouse_event mouse_event_deserialize(const char * buffer) {
    mouse_event event;

    const char * p = &buffer[0];
    if (*p++ != 'm' ||
        *p++ != 'o' ||
        *p++ != 'u' ||
        *p++ != 's' ||
        *p++ != 'e') {
        throw std::runtime_error("mouse_event_deserialize(): not mouse event");
    }

    event.type = *p++;
    event.code = *p++;
    memcpy(&event.value, p, sizeof event.value);

    return event;   
}
