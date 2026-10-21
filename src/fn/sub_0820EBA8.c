#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_0820E2A4(void);
void PoisonHand_Destroy(void);
s32 PoisonHand_InitializeFromScript(void *, void *);
void *PoisonHand_Create(void *p)
{
    void *r = Task_Create(8, 0x17c);
    if (r) {
        Task_SetCallbacks(r, sub_0820E2A4, PoisonHand_Destroy);
        if (PoisonHand_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
