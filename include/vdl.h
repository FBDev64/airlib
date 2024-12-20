#ifndef VDL_H
#define VDL_H

#include <GL/gl.h>

typedef enum {
   KB_KEY_UNKNOWN = 0,
   // Letters
   KB_KEY_A = 0x41,  // Maps to Windows VK codes
   KB_KEY_B = 0x42,
   KB_KEY_C = 0x43,
   KB_KEY_D = 0x44,
   KB_KEY_E = 0x45,
   KB_KEY_F = 0x46,
   KB_KEY_G = 0x47,
   KB_KEY_H = 0x48,
   KB_KEY_I = 0x49,
   KB_KEY_J = 0x4A,
   KB_KEY_K = 0x4B,
   KB_KEY_L = 0x4C,
   KB_KEY_M = 0x4D,
   KB_KEY_N = 0x4E,
   KB_KEY_O = 0x4F,
   KB_KEY_P = 0x50,
   KB_KEY_Q = 0x51,
   KB_KEY_R = 0x52,
   KB_KEY_S = 0x53,
   KB_KEY_T = 0x54,
   KB_KEY_U = 0x55,
   KB_KEY_V = 0x56,
   KB_KEY_W = 0x57,
   KB_KEY_X = 0x58,
   KB_KEY_Y = 0x59,
   KB_KEY_Z = 0x5A,
   // Numbers
   KB_KEY_0 = 0x30,
   KB_KEY_1 = 0x31,
   KB_KEY_2 = 0x32,
   KB_KEY_3 = 0x33,
   KB_KEY_4 = 0x34,
   KB_KEY_5 = 0x35,
   KB_KEY_6 = 0x36,
   KB_KEY_7 = 0x37,
   KB_KEY_8 = 0x38,
   KB_KEY_9 = 0x39,
   // Function keys
   KB_KEY_F1 = VK_F1,
   KB_KEY_F2 = VK_F2,
   KB_KEY_F3 = VK_F3,
   KB_KEY_F4 = VK_F4,
   KB_KEY_F5 = VK_F5,
   KB_KEY_F6 = VK_F6,
   KB_KEY_F7 = VK_F7,
   KB_KEY_F8 = VK_F8,
   KB_KEY_F9 = VK_F9,
   KB_KEY_F10 = VK_F10,
   KB_KEY_F11 = VK_F11,
   KB_KEY_F12 = VK_F12,
   // Special keys
   KB_KEY_ESCAPE = VK_ESCAPE,
   KB_KEY_SPACE = VK_SPACE,
   KB_KEY_ENTER = VK_RETURN,
   KB_KEY_TAB = VK_TAB,
   KB_KEY_BACKSPACE = VK_BACK,
   KB_KEY_INSERT = VK_INSERT,
   KB_KEY_DELETE = VK_DELETE,
   KB_KEY_RIGHT = VK_RIGHT,
   KB_KEY_LEFT = VK_LEFT,
   KB_KEY_DOWN = VK_DOWN,
   KB_KEY_UP = VK_UP,
   KB_KEY_HOME = VK_HOME,
   KB_KEY_END = VK_END,
   KB_KEY_CAPS_LOCK = VK_CAPITAL,
   KB_KEY_SCROLL_LOCK = VK_SCROLL,
   KB_KEY_NUM_LOCK = VK_NUMLOCK,
   KB_KEY_PRINT_SCREEN = VK_SNAPSHOT,
   KB_KEY_PAUSE = VK_PAUSE,
   // Modifier keys
   KB_KEY_SHIFT = VK_SHIFT,
   KB_KEY_CONTROL = VK_CONTROL,
   KB_KEY_ALT = VK_MENU
} KB_KEY;

typedef struct Keyboard {
   int (*isKeyPressed)(KB_KEY key);
} Keyboard;

typedef struct Win {
   // Video
   void (*create)(int width, int height, const char* title);
   void (*pollEvents)(void);
   void (*swapBuffers)(void);
   int (*shouldClose)(void);
   void (*drawText)(float x, float y, const char* text, unsigned char* color);
   void (*drawButton)(float x, float y, float width, float height, const char* label);
   void (*destroy)(void);
   int (*isButtonClicked)(float x, float y, float width, float height);
   void (*setBackgroundColor)(float red, float green, float blue, float alpha);
   void (*getOpenGLVersion)(void);
   GLuint (*loadTexture)(const char* filename);
   void (*drawTexture)(GLuint textureID, float x, float y, float width, float height);
   void (*checkGLError)(void);
   void (*createFramebuffer)(int width, int height);
   void (*beginFramebuffer)(int width, int height);
   void (*endFramebuffer)(void);
   GLuint (*getFramebufferTexture)(void);

   // Audio
   void (*playSound)(const char *filename);
   void (*stopSound)(void);

   // Input devices
   Keyboard* keyboard;
} Win;

Win* createWindowInstance(void);
int vdl_isKeyPressed(KB_KEY key);

#endif

/*
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
*/
