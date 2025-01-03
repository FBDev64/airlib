#include "include/vdl.h"
#include <GL/gl.h>
#include <stdio.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 800

int main() {
    const Win *window = createWindowInstance();

    window->create(WINDOW_WIDTH, WINDOW_HEIGHT, "VDL Demo");
    window->createFramebuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

    GLuint textureID = window->loadTexture("assets/ness.png"); // texture LOADED
    if (!textureID) {
        printf("Failed to load texture\n");
        return 1;
    }

    while (!window->shouldClose()) { // () in shouldClose is very important
        // Clear the screen first
        glClear(GL_COLOR_BUFFER_BIT);

        // text color
        unsigned char textColor[3] = {0, 0, 254};
        window->drawText(10, 10, "VDL drew this;", textColor, 2.0f); // scaled up text : 1.0 if default

        window->drawTexture(textureID, 100, 100, 200, 200); // texture DRAWN

        // key press
        window->drawText(10, 350, "Press E to make a button appear", textColor, 2.0f);

        if (window->keyboard->isKeyPressed(KB_KEY_E)) {
            window->drawButton(100, 300, 100, 50, "Play RUSH E");
        }

        // Draw framebuffer to screen
        window->drawTexture(window->getFramebufferTexture(), 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        window->swapBuffers();
        window->pollEvents();
    }

    window->destroy();
    return 0;
}
