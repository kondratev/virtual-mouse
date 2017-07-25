#ifndef MOUSE_EVENT
#define MOUSE_EVENT
// While this looks similar to the definitions found in <linux/input.h>, this
// intendeds to provide cross-platform events. When transmitting events from
// the server to clients, OS-specific events are translated into cross-platform
// events, sent across the network, and translated back
// into OS-specific events.

#define MOUSE_REF 0x0 // refresh
#define MOUSE_REL 0x1 // axis
#define MOUSE_BTN 0x2 // button

#define MOUSE_REL_X 0x1 // x axis
#define MOUSE_REL_Y 0x2 // y axis
#define MOUSE_REL_VWHEEL 0x3 // wheel

#define MOUSE_BTN_L 0x1 // left button
#define MOUSE_BTN_R 0x2 // right button
#define MOUSE_BTN_M 0x3 // middle button

#define MOUSE_NONE 0xFF

#define MOUSE_EVENT_SLEN (sizeof(mouse_event) + strlen("mouse") + 1)

// Only minimal mouse emulation is supported. Core features take precedence over
// supporting every configuration of mouse available.

struct mouse_event {
    unsigned char type { MOUSE_NONE };
    unsigned char code { MOUSE_NONE };
    long value { 0 };
};

mouse_event new_mouse_event(unsigned char type, unsigned char code, long value);
const char * mouse_event_serialize(const mouse_event & event);
mouse_event mouse_event_deserialize(const char * buffer);

#endif // MOUSE_EVENT