#include "script_prompt.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081E09A8(void);
void ScriptPrompt_Destroy(void);

void *ScriptPrompt_Create(void)
{
    void *r = Task_Create(0xb, sizeof(struct ScriptPromptScreen));
    if (r != 0) {
        Task_SetCallbacks(r, sub_081E09A8, ScriptPrompt_Destroy);
        if (ScriptPrompt_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
