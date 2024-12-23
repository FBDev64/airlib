#ifdef __linux__

#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#include "../../../include/stb_image.h"
#include "../../../include/stb_easy_font.h"

#include "../../../include/vdl.h"
#include "../../../include/audio.h"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE
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
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;

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

void create(int width, int height, const char* title) {
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

    XVisualInfo* visual = glXChooseVisual(display, screen, visualAttribs);
    if (!visual) {
        fprintf(stderr, "No suitable visual found\n");
        exit(EXIT_FAILURE);
    }

    Colormap colormap = XCreateColormap(display, root, visual->visual, AllocNone);

    XSetWindowAttributes swa;
    swa.colormap = colormap;
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask;

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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void endFramebuffer(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint getFramebufferTexture(void) {
    return frameBufferTexture;
}

GLuint loadTexture(const char* filename) {
    int width, height, channels;
    unsigned char* imageData = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
    if (!imageData) {
        printf("Failed to load texture: %s\n", filename);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

    stbi_image_free(imageData);
    return textureID;
}

void drawTexture(GLuint textureID, float x, float y, float width, float height) {
    glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width, y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width, y + height);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + height);
    glEnd();

    glPopAttrib();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void drawText(float x, float y, const char* text, unsigned char* color, float scale) {
    glPushMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // Set the text color using the provided RGB values
    glColor3ub(color[0], color[1], color[2]);

    static char vertex_buffer[99999];
    stb_easy_font_spacing(0.6f);

    // Use provided scale or default to 1.2f if 0
    float textScale = (scale > 0) ? scale : 1.2f;

    glTranslatef(x, y, 0);
    glScalef(textScale, textScale, 1.0f);

    int num_quads = stb_easy_font_print(0, 0, (char*)text, NULL, vertex_buffer, sizeof(vertex_buffer));

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, vertex_buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopAttrib();
    glPopMatrix();
}

void drawButton(float x, float y, float width, float height, const char* label) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    float textX = x + (width - strlen(label) * 8) / 2;
    float textY = y + (height - 12) / 2;
    unsigned char textColor[3] = {0, 0, 0};

    float scale = 1.0f;

    drawText(textX, textY, label, textColor, scale);
}

int isButtonClicked(float x, float y, float width, float height) {
    XEvent event;
    XNextEvent(display, &event);

    if (event.type == ButtonPress) {
        if (event.xbutton.x >= x && event.xbutton.x <= x + width &&
            event.xbutton.y >= y && event.xbutton.y <= y + height) {
            return 1;  // Button was clicked
        }
    }
    return 0;  // Button was not clicked
}

void checkGLError() {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        printf("OpenGL error: 0x%X\n", err);
    }
}

void destroy(void) {
    glXMakeCurrent(display, None, NULL);
    glXDestroyContext(display, glxContext);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

Win* createWindowInstance(void) {
    static Keyboard kb = {
        .isKeyPressed = vdl_isKeyPressed
    };

    static Win window = {
        .create = create,
        .pollEvents = pollEvents,
        .swapBuffers = swapBuffers,
        .shouldClose = shouldClose,
        .drawText = drawText,
        .drawButton = drawButton,
        .isButtonClicked = isButtonClicked,
        .destroy = destroy,
        .loadTexture = loadTexture,
        .drawTexture = drawTexture,
        .checkGLError = checkGLError,
        .createFramebuffer = createFramebuffer,
        .beginFramebuffer = beginFramebuffer,
        .endFramebuffer = endFramebuffer,
        .getFramebufferTexture = getFramebufferTexture,
        .keyboard = &kb
    };
    return &window;
}

#endif