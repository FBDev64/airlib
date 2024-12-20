#if defined(__WIN32)

#define STB_IMAGE_IMPLEMENTATION
#include "../../../include/stb_image.h"
#include "../../../include/stb_easy_font.h"
#include "../../../include/vdl.h"
#include "../../../include/audio.h"

#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <stdio.h>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

#ifndef GL_FRAMEBUFFER
#define GL_FRAMEBUFFER 0x8D40
#endif

#ifndef GL_COLOR_ATTACHMENT0
#define GL_COLOR_ATTACHMENT0 0x8CE0
#endif

// Global variables
static HWND hwnd;
static HDC hdc;
static HGLRC hglrc;
static int should_close = 0;
static GLuint frameBuffer;
static GLuint frameBufferTexture;

// Function pointers for framebuffer operations
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            should_close = 1;
            PostQuitMessage(0);
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

__declspec(dllexport) void pollEvents(void) {
    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            should_close = 1;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

__declspec(dllexport) int shouldClose(void) {
    return should_close;
}

void create(int width, int height, const char* title) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "OpenGL_Window";
    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        0, wc.lpszClassName, title, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, wc.hInstance, NULL
    );

    hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 8, 0,
        PFD_MAIN_PLANE, 0, 0, 0, 0
    };

    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);

    // Load OpenGL extensions
    glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
    glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
    glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");

    // OpenGL initialization
    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

__declspec(dllexport) void swapBuffers(void) {
    SwapBuffers(hdc);
}

__declspec(dllexport) void createFramebuffer(int width, int height) {
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glGenTextures(1, &frameBufferTexture);
    glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

__declspec(dllexport) void beginFramebuffer(int width, int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

__declspec(dllexport) void endFramebuffer(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

__declspec(dllexport) GLuint getFramebufferTexture(void) {
    return frameBufferTexture;
}

__declspec(dllexport) GLuint loadTexture(const char* filename) {
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

__declspec(dllexport) void drawTexture(GLuint textureID, float x, float y, float width, float height) {
    glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width, y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width, y + height);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y + height);
    glEnd();

    glPopAttrib();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

__declspec(dllexport) void drawText(float x, float y, const char* text, unsigned char* color) {
    glPushMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glColor3ub(color[0], color[1], color[2]);

    static char vertex_buffer[99999];
    stb_easy_font_spacing(0.6f);
    float scale = 1.2f;

    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1.0f);

    int num_quads = stb_easy_font_print(0, 0, (char*)text, NULL, vertex_buffer, sizeof(vertex_buffer));

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, vertex_buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopAttrib();
    glPopMatrix();
}

__declspec(dllexport) void destroy(void) {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hglrc);
    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);
}

__declspec(dllexport) void drawButton(float x, float y, float width, float height, const char* label) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    float textX = x + (width - strlen(label) * 8) / 2;
    float textY = y + (height - 12) / 2;
    unsigned char textColor[3] = {0, 0, 0};
    drawText(textX, textY, label, textColor);
}

__declspec(dllexport) int isButtonClicked(float x, float y, float width, float height) {
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
        POINT p;
        GetCursorPos(&p);
        ScreenToClient(hwnd, &p);

        if (p.x >= x && p.x <= x + width && p.y >= y && p.y <= y + height) {
            return 1;
        }
    }
    return 0;
}

__declspec(dllexport) void checkGLError() {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        printf("%s", err);
    }
}

__declspec(dllexport) Win* createWindowInstance(void) {
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
        .playSound = playSound,
        .stopSound = stopSound,
        .keyboard = &kb
    };
    return &window;
}

#endif
