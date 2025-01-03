#ifdef __linux__

#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#include "../../../include/stb_image.h"
#include "../../../include/stb_easy_font.h"

#include "../../../include/vdl.h"
#include "../../../include/audio.h"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

// Global variables
static Display *display;
static Window window;
static GLXContext glxContext;
static Atom wmDeleteMessage;
static int should_close = 0;
static GLuint frameBuffer;
static GLuint frameBufferTexture;

// Function pointers for framebuffer operations
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = NULL;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = NULL;

// Helper to load OpenGL function pointers
void loadGLFunctions(void) {
    glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glXGetProcAddressARB((const GLubyte*)"glGenFramebuffers");
    glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glXGetProcAddressARB((const GLubyte*)"glBindFramebuffer");
    glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glXGetProcAddressARB((const GLubyte*)"glFramebufferTexture2D");

    if (!glGenFramebuffers || !glBindFramebuffer || !glFramebufferTexture2D) {
        fprintf(stderr, "Failed to load OpenGL framebuffer functions.\n");
        exit(EXIT_FAILURE);
    }
}

// Function to poll X11 events
void pollEvents(void) {
    XEvent event;
    while (XPending(display)) {
        XNextEvent(display, &event);
        if (event.type == ClientMessage) {
            should_close = 1;
        }
    }
}

int shouldClose(void) {
    return should_close;
}

void create(int width, int height, const char *title) {
    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Failed to open X display\n");
        exit(EXIT_FAILURE);
    }

    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);

    int visualAttribs[] = {
        GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None
    };

    XVisualInfo *visual = glXChooseVisual(display, screen, visualAttribs);
    if (!visual) {
        fprintf(stderr, "No suitable visual found\n");
        exit(EXIT_FAILURE);
    }

    Colormap colormap = XCreateColormap(display, root, visual->visual, AllocNone);

    XSetWindowAttributes swa = {
        .colormap = colormap,
        .event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask
    };

    window = XCreateWindow(
        display, root, 0, 0, width, height, 0, visual->depth, InputOutput,
        visual->visual, CWColormap | CWEventMask, &swa
    );

    XStoreName(display, window, title);

    wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wmDeleteMessage, 1);

    XMapWindow(display, window);

    glxContext = glXCreateContext(display, visual, NULL, GL_TRUE);
    glXMakeCurrent(display, window, glxContext);

    // OpenGL initialization
    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void swapBuffers(void) {
    glXSwapBuffers(display, window);
}

void createFramebuffer(int width, int height) {
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glGenTextures(1, &frameBufferTexture);
    glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void beginFramebuffer(int width, int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
}

void endFramebuffer(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint getFramebufferTexture(void) {
    return frameBufferTexture;
}

void destroy(void) {
    if (frameBuffer) {
        glDeleteFramebuffers(1, &frameBuffer);
    }
    if (frameBufferTexture) {
        glDeleteTextures(1, &frameBufferTexture);
    }
    glXMakeCurrent(display, None, NULL);
    glXDestroyContext(display, glxContext);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

Win* createWindowInstance(void) {
    static Win window = {
        .create = create,
        .pollEvents = pollEvents,
        .swapBuffers = swapBuffers,
        .shouldClose = shouldClose,
        .destroy = destroy,
        .createFramebuffer = createFramebuffer,
        .beginFramebuffer = beginFramebuffer,
        .endFramebuffer = endFramebuffer,
        .getFramebufferTexture = getFramebufferTexture,
    };
    return &window;
}

#endif
