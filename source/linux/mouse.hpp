#ifdef __unix__
#ifndef MOUSE
#define MOUSE

#include <iostream>
#include <string>

#define MOUSE_NAME    "BNP Virtual Mouse"
#define MOUSE_VENDOR  0x0000
#define MOUSE_PRODUCT 0x0000
#define MOUSE_VERSION 0x0001
#define MOUSE_NODE    "event"

std::string get_vmouse_path();

// If "#include <linux/input_event.h>" is included, it throws an error stating
// "#include <asm/cache.h>" cannot be found. The solution online is to create
// a symlink from "asm-generic" to "asm," but my disto already has the
// directory "asm."
#include <asm/types.h>
#include <sys/time.h>

// "input_event" as defined by <linux/input_event.h>
struct input_event {
    struct timeval time;
    __u16 type;
    __u16 code;
    __s32 value;
};

#include "mouse_event.hpp"
// Converts unix to standard mouse event
mouse_event input_event_to_mouse_event(const input_event & input);
// Converts standard to unix mouse event
input_event mouse_event_to_input_event(const mouse_event & input);


mouse_event read_hmouse(int fd);


#endif // MOUSE
#endif // __unix__