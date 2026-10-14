#include "script_selector.h"
#define ScriptSelector_CreateTask     sub_080422DC
#define ScriptSelector_UpdateTask     sub_080421E8
#define ScriptSelector_InitializeTask sub_08042278
s32 ScriptSelector_UpdateTask(struct ScriptSelector *);
s32 ScriptSelector_InitializeTask(struct ScriptSelector *);
struct ScriptSelector *ScriptSelector_CreateTask(void)
{
    struct ScriptSelector *selector = Task_Create(4, sizeof(struct ScriptSelector));
    if (selector) {
        Task_SetCallbacks(selector, (void (*)(void *))ScriptSelector_UpdateTask, 0);
        if (ScriptSelector_InitializeTask(selector) >= 0)
            goto ready;
        Task_Kill(selector);
        return 0;
    }
ready:
    return selector;
}
