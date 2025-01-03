#ifdef __linux__

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../../include/vdl.h"

// Keyboard state structure to store key states
static int keyStates[256] = {0};

// Global display and window variables
static Display* display;
static Window window;

// Function to initialize the input system
void initializeInput(void) {
    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Failed to open X display\n");
        exit(EXIT_FAILURE);
    }

    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);

    XSetWindowAttributes swa;
    swa.event_mask = KeyPressMask | KeyReleaseMask;

    window = XCreateWindow(
        display, root, 0, 0, 640, 480, 0, CopyFromParent, InputOutput,
        CopyFromParent, CWEventMask, &swa
    );

    XMapWindow(display, window);
}

// Function to process keyboard events and update the key states
void pollKeyboardEvents(void) {
    XEvent event;
    while (XPending(display)) {
        XNextEvent(display, &event);

        if (event.type == KeyPress) {
            KeySym key = XKeycodeToKeysym(display, event.xkey.keycode, 0);
            // Update key state (pressed)
            if (key >= XK_A && key <= XK_Z) {
                keyStates[key] = 1;
            }
        } else if (event.type == KeyRelease) {
            KeySym key = XKeycodeToKeysym(display, event.xkey.keycode, 0);
            // Update key state (released)
            if (key >= XK_A && key <= XK_Z) {
                keyStates[key] = 0;
            }
        }
    }
}

// Function to check if a key is pressed
int vdl_isKeyPressed(KB_KEY key) {
    return keyStates[(int)key];
}

// Function to destroy the input system and free resources
void destroyInput(void) {
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

#endif
