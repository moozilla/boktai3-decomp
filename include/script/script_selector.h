#ifndef GUARD_SCRIPT_SELECTOR_H
#define GUARD_SCRIPT_SELECTOR_H
/* Tentative script-entry selector. The task allocation is 0x30 bytes. */
#include "engine/engine.h"
#include "script/script_virtual_machine.h"
#include "engine/input.h"
struct ScriptSelector {
    struct Task task;
    u8 *scriptStart;
    u32 value;
    u32 selectedScript;
    s32 index;
    s32 repeatDelay;
    s32 lastDirection;
};
void ScriptSelector_MoveSelection(struct ScriptSelector *, s32 delta);
s32 ScriptSelector_UpdateTask(struct ScriptSelector *);
s32 ScriptSelector_InitializeTask(struct ScriptSelector *);
struct ScriptSelector *ScriptSelector_CreateTask(void);
u32 Heap_IsEndGuardValid(void);
#endif
