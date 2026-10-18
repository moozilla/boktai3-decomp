#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081E056C(void *);
void Task_Kill(void *);
void StartPrompt_Update(void);
void ScriptMessage_Destroy(void);

void *ScriptMessage_Create(void)
{
    void *r = Task_Create(0xb, 1228);
    if (r != 0) {
        Task_SetCallbacks(r, StartPrompt_Update, ScriptMessage_Destroy);
        if (sub_081E056C(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
