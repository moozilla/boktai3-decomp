#include "global.h"
extern u32 gUnk_020000D8;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 ActorPool_InitManager(void *);
void Task_Kill(void *);
void ActorPool_UpdateCallback(void);
void ActorPool_DestroyCallback(void);

void *ActorPool_CreateManager(void)
{
    void *p;
    if (gUnk_020000D8 != 0)
        return (void *)gUnk_020000D8;
    p = Task_Create(8, 0x38);
    if (p != 0) {
        Task_SetCallbacks(p, ActorPool_UpdateCallback, ActorPool_DestroyCallback);
        if (ActorPool_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
