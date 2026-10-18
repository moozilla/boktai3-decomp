#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C0390(void *);
void Task_Kill(void *);
void TetherActor_UpdateRequestIndicator(void *);
void TetherActor_DestroyRequestIndicator(void *);
void *TetherActor_CreateRequestIndicator(void)
{
    void *p = Task_Create(9, 0xac);
    if (p != 0) {
        Task_SetCallbacks(p, TetherActor_UpdateRequestIndicator,
                          TetherActor_DestroyRequestIndicator);
        if (sub_081C0390(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
