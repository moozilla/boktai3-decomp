#ifndef GUARD_SCRIPT_SELECTOR_H
#define GUARD_SCRIPT_SELECTOR_H
/* Tentative script-entry selector. The task allocation is 0x30 bytes. */
#include "engine.h"
struct ScriptSelector {
    u8 task[0x18];
    u32 scriptStart;
    u32 value;
    u32 selectedScript;
    s32 index;
    s32 repeatDelay;
    s32 lastDirection;
};
#endif
