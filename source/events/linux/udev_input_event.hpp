#ifndef LINUX_INPUT_EVENT
#define LINUX_INPUT_EVENT

#ifdef __unix__

struct mouse_event;

#include <stdint.h>
#include <sys/time.h>
#include <linux/input-event-codes.h>

// This is intended to mimic input_event found in <linux/input.h>. This allows
// the application to read/write from a udev device. This makes it incredibly
// convenient to convert this to a generic event.
struct udev_input_event {
    struct timeval time;
    uint16_t type { 0 };
    uint16_t code { 0 };
    int32_t value { 0 };

    // Defines default constructor
    udev_input_event() = default;
    // Converts mouse_event to udev_input_event
    udev_input_event(const mouse_event&);
    // Converts mouse_event to udev_input_event
    void operator=(const mouse_event&);

    // This is used by the client to modify a cursor/keyboard udev device.
    // Since udev_input_event has binary compatibility with input_event
    // the memory in udev_input_event is written to the udev device.
    void write(int fd);
};

#endif // __unix__
#endif // LINUX_MOUSE_EVENT