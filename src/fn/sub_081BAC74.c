#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 Registry3F74_InitFromScript(void *, void *);
void Task_Kill(void *);
void Registry3F74_OnUpdate(void);
void Registry3F74_OnDestroy(void);
void *Registry3F74_Create(void *a)
{
    void *p = Task_Create(8, 0x268);
    if (p) {
        Task_SetCallbacks(p, Registry3F74_OnUpdate, Registry3F74_OnDestroy);
        if (Registry3F74_InitFromScript(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
