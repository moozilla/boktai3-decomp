#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 ScenePaletteFade_InitFromScript(void *, void *);
void Task_Kill(void *);
void ScenePaletteFade_Update(void *);
void ScenePaletteFade_Destroy(void *);
void *ScenePaletteFade_Create(void *a)
{
    void *p = Task_Create(0xb, 0x38);
    if (p != 0) {
        Task_SetCallbacks(p, ScenePaletteFade_Update, ScenePaletteFade_Destroy);
        if (ScenePaletteFade_InitFromScript(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
