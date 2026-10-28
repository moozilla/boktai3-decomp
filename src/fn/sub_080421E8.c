#include "script_selector.h"
#include "input.h"
void ScriptSelector_MoveSelection(struct ScriptSelector *, s32);
u32 Heap_IsEndGuardValid(void);
s32 ScriptSelector_UpdateTask(struct ScriptSelector *selector)
{
    u16 *keys = gPadState;
    u32 held = keys[0];
    u32 vertical = held & 0xC0;
    if (vertical) {
        u32 up = held & 0x40;
        s32 step;
        step = 1;
        if (up)
            step -= 2;
        if (selector->lastDirection == step) {
            selector->repeatDelay--;
            if (selector->repeatDelay < 0) {
                ScriptSelector_MoveSelection(selector, step);
                selector->repeatDelay = 4;
            }
        } else {
            ScriptSelector_MoveSelection(selector, step);
            selector->repeatDelay = 20;
            selector->lastDirection = step;
        }
    } else {
        selector->lastDirection = vertical;
    }
    if (keys[1] & 2) {
        s32 oldIndex;
        do {
            oldIndex = selector->index;
            ScriptSelector_MoveSelection(selector, 1);
        } while (oldIndex != selector->index);
    }
    if (keys[2] & 1) {
        Script_Call(selector->selectedScript, 0);
        Task_Kill(selector);
    }
    Heap_IsEndGuardValid();
    return 0;
}
