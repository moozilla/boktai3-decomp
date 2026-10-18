#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_0820107C(void *);
void Task_Kill(void *);
void sub_08200C94(void);
void PlayerClone_DestroyPool(void);

void *PlayerClone_CreatePool(void)
{
    void *p = Task_Create(8, 0xa2c);
    if (p) {
        Task_SetCallbacks(p, sub_08200C94, PlayerClone_DestroyPool);
        if (sub_0820107C(p) < 0) {
            Task_Kill(p);
            return 0;
        }
        return p;
    }
}
