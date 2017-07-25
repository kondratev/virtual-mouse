#include <linux/init.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include "../mouse.h"

static struct input_dev *vmouse_dev;
static int vmouse_dev_registered;

static int __init vmouse_dev_init(void) {
    if ((vmouse_dev = input_allocate_device()) == NULL) {
        printk("error: (vmouse_dev = input_allocate_device()) == NULL\n");
        return 0;
    }
    vmouse_dev->name = MOUSE_NAME;
    vmouse_dev->id.bustype = BUS_VIRTUAL;   // virtual bus
    vmouse_dev->id.vendor  = MOUSE_VENDOR;  // [xxxx:0000]
    vmouse_dev->id.product = MOUSE_PRODUCT; // [0000:xxxx]
    vmouse_dev->id.version = MOUSE_VERSION; // ?
    vmouse_dev->evbit[0] =
        BIT_MASK(EV_KEY) | // buttons (i.e left, right, middle)
        BIT_MASK(EV_REL);  // relative axis (i.e, x, y, scrollwheel)
    vmouse_dev->keybit[BIT_WORD(BTN_MOUSE)] =
        BIT_MASK(BTN_LEFT) |  // left button
        BIT_MASK(BTN_RIGHT) | // right button
        BIT_MASK(BTN_MIDDLE); // middle button
    vmouse_dev->relbit[0] =
        BIT_MASK(REL_X) |    // x axis
        BIT_MASK(REL_Y) |    // y axis
        BIT_MASK(REL_WHEEL); // scrollwheel axis
    // Determines if the virtual mouse can be registered. If it cannot, then
    // the device should be released. The registered status is stored to
    // prevent calling input_unregister_device on an unregistered device.
    // Doing so may crash the system.
    if ((vmouse_dev_registered = input_register_device(vmouse_dev))) {
        printk("error: (vmouse_dev_registered = input_register_device(vmouse_dev))\n");
        input_free_device(vmouse_dev);
    }
    return 0;
}
static void __exit vmouse_dev_exit(void) {
    // Determines if the virtual mouse was registered. If it is, then the
    // device should be unregistered. One input_unregister_device is called,
    // input_free_device should not be called. Doing so may crash the system.
    if (!vmouse_dev_registered) {
        input_unregister_device(vmouse_dev);
    }
}
module_init(vmouse_dev_init);
module_exit(vmouse_dev_exit);