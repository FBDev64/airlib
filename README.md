<p align="center">
    <a><img alt="C" src="https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white" /></a>
    <a><img alt="OpenGL" src="https://img.shields.io/badge/OpenGL-%23FFFFFF.svg?style=for-the-badge&logo=opengl"></a>
    <a><img alt="linux" src="https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black">
</p>

# VDL

**V**ideo **D**evelopment **L**ibrary is a low-level, cross-platform development library for OpenGL written in portable C (C99).

## Features

* Written in portable C (C99)
* [Zlib license](./LICENSE): free for any project, including commercial ones
* Cross-platform : *Windows* and *X11*
* Handles I/O (Keyboard and Mouse inputs)
* Handles graphics (window creation, frame buffer, image drawing)
* Handles audio (play sound, stop sound)

Homepage : [adamonair.neocities.org/vdl](https://adamonair.neocities.org/vdl), hosted by Neocities.

## Install


### UNIX
```bash
git clone https://github.com/AdamOnAir/pkg # pkg is the repo containing VDL installer
cd pkg
chmod +x installer.sh
sudo ./installer.sh
```

### Windows
- Go to [pkg](https://github.com/AdamOnAir/pkg/releases/)
- Download `setup-vdl.exe`
- Execute

## Demo

The following code is an demo of VDL usage.
```c
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
```

## License

```text
Copyright (C) 2024 Ellouze Adam <elzadam11@tutamail.com>
  
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
  
1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required. 
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
```
