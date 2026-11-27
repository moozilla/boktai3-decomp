#ifndef GUARD_INCLUDE_UI_DIALOGUE_TEXT_WINDOW_MANAGER_H
#define GUARD_INCLUDE_UI_DIALOGUE_TEXT_WINDOW_MANAGER_H

#include "global.h"

struct TextWindow;

struct WindowManager {
    u8 pad0[0x14];
    void *script;
    s32 nextId;
    struct TextWindow *head;
};

typedef char WindowManagerSizeCheck[sizeof(struct WindowManager) == 0x20 ? 1 : -1];

#endif
