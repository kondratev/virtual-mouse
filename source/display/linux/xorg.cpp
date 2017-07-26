#include <iostream>
#include <stdexcept>
#include <X11/X.h>
#include <X11/Xlib.h>

struct xorg_cursor_control {
    Display * display;
    Window window;
    Cursor cursor;
    int screen;

     xorg_cursor_control();
    ~xorg_cursor_control();
    void update();
    void grab();
    void ungrab();

private:
    void init_cursor();
    void init_display();
};


void xorg_cursor_control::update() {
    // Since the pointer is "grabbed" by the application, events for the pointer
    // are filling up the event queue. This will release some of those events.
    XAllowEvents(this->display, SyncPointer, CurrentTime);
    // Flushes all events in the queue. Those flushed events are then procssed
    // by the xorg server.
    XSync(this->display, True);
}

void xorg_cursor_control::grab() {
    // "Grabs" control of the pointer. Pointer events are redirected only to
    // this application. This is unecessary as the server uses the driver to
    // detect movement of the pointer. This is used to prevent pointer
    // interaction when the pointer is interacting with a client machine.
    XGrabPointer(
        this->display,      // Display
        this->window,       // Root Window
        True,               // Take Ownership
        PointerMotionMask | ButtonPressMask,
                             // Specified Events
        GrabModeSync,        // Mouse Events
        GrabModeAsync,       // Keyboard Events
        None,                // Do not confine
        this->cursor,        // Pointer
        CurrentTime          // Current Time
    );
}

void xorg_cursor_control::ungrab() {
    // "Ungrabs" the pointer. Pointer events are now directed to the xorg
    // server. This allows the pointer to interact with the host machine.
    XUngrabPointer(this->display, CurrentTime);
}

void xorg_cursor_control::init_cursor() {
    XColor color = { 0 };
    const char data[] = { 0 };
    // The xorg documentation is poor. Presumably this creates a pixmap with
    // the specified data. The pixmap should be 1x1 black pixels. The goal is
    // an "invisible" pointer, so the server looks like it doesn't have a pointer.
    Pixmap pixmap = XCreateBitmapFromData(this->display, this->window, data, 1, 1);
    // The xorg documentation is poor. Presumably this creates a cursor with
    // the generated pixmap. Apparently the pixmap doesn't specify the color.
    // This places the cursor at (0,0).
    this->cursor = XCreatePixmapCursor(this->display, pixmap, pixmap, &color, &color, 0, 0);
    // Releases the pixmap
    XFreePixmap(this->display, pixmap);
}

void xorg_cursor_control::init_display() {
    // Determines if we can open the Xorg display. This is required if one
    // wishes to use a server with Xorg. Eventually a Wayland server will
    // be written to control the display.
    if (!(this->display = XOpenDisplay(nullptr))) {
        throw std::runtime_error("!(display = XOpenDisplay(nullptr))");
    }
    // Retrieves the default screen from the display
    this->screen = DefaultScreen(this->display);
    // Retrieves the root window from the display
    this->window = XRootWindow(this->display, this->screen);
}

xorg_cursor_control::xorg_cursor_control() {
    // Initializes the display
    this->init_display();
    // Initializes the cursor
    this->init_cursor();
}

xorg_cursor_control::~xorg_cursor_control() {
    // "Ungrabs" the cursor
    this->ungrab();
    // Releases the cursor
    XFreeCursor(this->display, this->cursor);
}