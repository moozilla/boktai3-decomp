#include "script_selector.h"

void ScriptSelector_MoveSelection(struct ScriptSelector *selector, s32 delta)
{
    u32 value = 0;
    u32 procedure = 0;
    s32 index;
    selector->index += delta;
    if (selector->index < 0)
        selector->index = procedure;
    Script_SetPc(selector->scriptStart);
    for (index = 0; index <= selector->index; index++) {
        if (Script_GetPc() == 0) {
            selector->index = index;
            break;
        }
        value = Script_ReadValueSafe(Script_GetPc());
        procedure = Script_ReadValue(Script_GetPc());
    }
    selector->value = value;
    selector->selectedScript = procedure;
}

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

s32 ScriptSelector_InitializeTask(struct ScriptSelector *selector)
{
    u8 *resource;
    gBgFadeLevel = 0x40;
    if (!Script_SeekToKeyword('s'))
        return -1;
    selector->scriptStart = Script_GetPc();
    selector->index = 0;
    selector->lastDirection = 0;
    ScriptSelector_MoveSelection(selector, 0);
    resource = ScriptProc_Start(0x92B3, 0x519C);
    CpuSet(resource + 0x1F4, gBgPaletteBank15, 0x04000008);
    return 0;
}

struct ScriptSelector *ScriptSelector_CreateTask(void)
{
    struct ScriptSelector *selector = Task_Create(4, sizeof(struct ScriptSelector));
    if (selector) {
        Task_SetCallbacks(selector, (void (*)(void *))ScriptSelector_UpdateTask, 0);
        if (ScriptSelector_InitializeTask(selector) < 0) {
            Task_Kill(selector);
            return 0;
        }
    }
    return selector;
}

void SceneActorRegistry_IgnoreEntry1B77(void)
{
}

void SceneActorRegistry_IgnoreEntryD562(void)
{
}

void SceneActorRegistry_IgnoreEntry3D0C(void)
{
}
