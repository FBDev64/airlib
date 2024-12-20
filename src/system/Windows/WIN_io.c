#if defined(__WIN32)

#include <windows.h>
#include <stdio.h>
#include "../../../include/vdl.h"

static HHOOK keyHook;
static int keyStates[256] = {0};

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        KBDLLHOOKSTRUCT* pStruct = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN) {
            keyStates[pStruct->vkCode] = 1;
        }
        else if (wParam == WM_KEYUP) {
            keyStates[pStruct->vkCode] = 0;
        }
    }
    return CallNextHookEx(keyHook, nCode, wParam, lParam);
}

__declspec(dllexport) int vdl_isKeyPressed(KB_KEY key) {
    static int initialized = 0;
    if (!initialized) {
        keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
        initialized = 1;
    }
    return keyStates[(int)key];
}

#endif
