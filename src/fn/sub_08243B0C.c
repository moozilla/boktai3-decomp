#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void Player_UpdateLinkCore(void *);
void Player_OnDestroy(void *);
s32 Player_OnCreate(void *, u32, u32);

void *sub_08243B0C(u32 a, u32 b)
{
    void *task = Task_Create(6, 0xcac);
    if (task != NULL) {
        Task_SetCallbacks(task, Player_UpdateLinkCore, Player_OnDestroy);
        if (Player_OnCreate(task, a, b) < 0) {
            Task_Kill(task);
            return NULL;
        }
    }
    return task;
}
