#ifdef __unix__
#ifndef MOUSE
#define MOUSE

#include <iostream>
#include <string>

#include "mouse_definitions.h"

std::string get_vmouse_path();

// If "#include <linux/input_event.h>" is included, it throws an error stating
// "#include <asm/cache.h>" cannot be found. The solution online is to create
// a symlink from "asm-generic" to "asm," but my disto already has the
// directory "asm."
#include <asm/types.h>
#include <sys/time.h>

// "input_event" as defined by <linux/input_event.h>
struct input_event {
    struct timeval time { 0 };
    __u16 type { 0 };
    __u16 code { 0 };
    __s32 value { 0 };
};

#include "mouse_event.hpp"
// Converts unix to standard mouse event
mouse_event input_event_to_mouse_event(const input_event & input);
// Converts standard to unix mouse event
input_event mouse_event_to_input_event(const mouse_event & input);


input_event read_hmouse(int fd);


#endif // MOUSE
#endif // __unix__
