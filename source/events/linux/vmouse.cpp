#include "vmouse.hpp"

#include <libudev.h>
#include <string.h>
#include <stdexcept>
std::string get_vmouse_path() {
    std::string mouse_path;
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices;
    struct udev_list_entry *entry;

    // Initializes the udev context
    if (!(udev = udev_new())) {
        throw std::runtime_error("udev(): null");
    }
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
            if (parent_name && !strcmp(parent_name, "BNP Virtual Mouse") &&
                device_node &&  strstr(device_node, "event")) {
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