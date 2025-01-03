#include "include/vdl.h"
#include <GL/gl.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main() {
    Win* window = createWindowInstance();
    
    window->create(WINDOW_WIDTH, WINDOW_HEIGHT, "VDL Game Engine Demo");
    
    //window->createFramebuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    while (!window->shouldClose()) {
        // Clear the screen first
        glClear(GL_COLOR_BUFFER_BIT);

        //window->beginFramebuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

        unsigned char redColor[3] = {255, 0, 0};  // Red text
        // window->drawText(100, 100, "Hello World", redColor, 2.0f);  // Scaled up text

        // window->endFramebuffer();

        // window->drawTexture(window->getFramebufferTexture(), 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        window->swapBuffers();
        window->pollEvents();
    }

    window->destroy();
    return 0;
}
