#ifndef LINUX_INPUT_EVENT
#define LINUX_INPUT_EVENT

#ifdef __unix__

#include <linux/input-event-codes.h>

struct mouse_event;

#include <stdint.h>
#include <sys/time.h>
struct udev_input_event {
    struct timeval time;
    uint16_t type { 0 };
    uint16_t code { 0 };
    int32_t value { 0 };

    udev_input_event() = default;
    udev_input_event(const mouse_event&);
    void operator=(const mouse_event&);
    void write(int mouse);
};

#endif // __unix__
#endif // LINUX_MOUSE_EVENT