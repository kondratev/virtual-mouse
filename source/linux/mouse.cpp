#ifdef __unix__
#include "mouse.hpp"

#include <libudev.h>
#include <string.h>
std::string get_vmouse_path() {
    std::string mouse_path;
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices;
    struct udev_list_entry *entry;

    // Initializes the udev context
    udev = udev_new();
    // Initializes an enumeration object. In this case, we don't want to
    // filter the enumeration object because the filtration needs to occur
    // with both a node and parent node.
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);
    
    udev_list_entry_foreach(entry, devices) {
        const char * device_path;
        const char * device_node;
        const char * parent_name;
        struct udev_device * device;
        struct udev_device * parent;

        // Retrieves the device and parent device
        device_path = udev_list_entry_get_name(entry);
        device = udev_device_new_from_syspath(udev, device_path);
        parent = udev_device_get_parent(device);

        // It's possible that the device does not have a parent device. Since
        // we don't know that this is the device we are looking for, we cannot
        // assume it has a parent.
        if (parent) {
            parent_name = udev_device_get_sysattr_value(parent, "name");
            device_node = udev_device_get_devnode(device);
            // Determines if the device and parent have the "correct" properties.
            // We only want to send events to a device that has the appropriate
            // name. We also don't want to send events to the mouse device.
            if (parent_name && !strcmp(parent_name, MOUSE_NAME) &&
                device_node &&  strstr(device_node, MOUSE_NODE)) {
                mouse_path = device_node;
            }
        }
        // Releases the deevice
        udev_device_unref(device);
    }
    // Releases the enumerater and udev context
    udev_enumerate_unref(enumerate);
    udev_unref(udev);
    // Returns the mouse path
    return mouse_path;
}

#include <linux/input-event-codes.h>
mouse_event input_event_to_mouse_event(const input_event & input) {
    mouse_event mouse;
    // Converts supported mouse event types
    if (input.type == EV_SYN) mouse.type = MOUSE_REFRESH;
    else if (input.type == EV_REL) mouse.type = MOUSE_REL;
    else if (input.type == EV_KEY) mouse.type = MOUSE_BTN;
    else {
        mouse.type = input.type;
        mouse.code = input.code;
        mouse.value = input.value;
    }
    // Converts supported mouse axis
    if (mouse.type == MOUSE_REL) {
        if (input.code == REL_X) mouse.code = MOUSE_REL_X;
        if (input.code == REL_Y) mouse.code = MOUSE_REL_Y;
        if (input.code == REL_WHEEL) mouse.code = MOUSE_REL_VWHEEL;
    // Converts supported mosue buttons
    } else if (mouse.type == MOUSE_BTN) {
        if (input.code == BTN_LEFT) mouse.code = MOUSE_BTN_L;
        if (input.code == BTN_RIGHT) mouse.code = MOUSE_BTN_R;
        if (input.code == BTN_MIDDLE) mouse.code = MOUSE_BTN_M;
    }
    if (mouse.type == MOUSE_REFRESH ||
        mouse.type == MOUSE_REL ||
        mouse.type == MOUSE_BTN) {
        // Converts the value
        mouse.value = input.value;
    }
    // Returns mouse event
    return mouse;
}

#include <linux/input-event-codes.h>
input_event mouse_event_to_input_event(const mouse_event & mouse) {
    input_event input;
    // Converts supported mouse event types
    if (mouse.type == MOUSE_REFRESH) input.type = EV_SYN;
    else if (mouse.type == MOUSE_REL) input.type = EV_REL;
    else if (mouse.type == MOUSE_BTN) input.type = EV_KEY;
    else {
        input.type = mouse.type;
        input.code = mouse.code;
        input.value = input.value;
    }
    // Converts supported mouse axis
    if (input.type == EV_REL) {
        if (mouse.code == MOUSE_REL_X) input.code = REL_X;
        if (mouse.code == MOUSE_REL_Y) input.code = REL_Y;
        if (mouse.code == MOUSE_REL_VWHEEL) input.code = REL_WHEEL;
    // Converts supported mouse buttons
    } else if (input.type == EV_KEY) {
        if (mouse.code == MOUSE_BTN_L) input.code = BTN_LEFT;
        if (mouse.code == MOUSE_BTN_R) input.code = BTN_RIGHT;
        if (mouse.code == MOUSE_BTN_M) input.code = BTN_MIDDLE;
    }
    // Converts the value
    input.value = mouse.value;
    // Returns input event
    return input;
}

#include <unistd.h>
mouse_event read_hmouse(int fd) {
    input_event input;
    int count = read(fd, &input, sizeof(input_event));
    return input_event_to_mouse_event(input);
}


#endif // __unix__
