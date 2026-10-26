#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SlashEffect_InitManager(u8 *);
void Task_Kill(u8 *);
void sub_0812BFBC(void);
void PursuingActors_ReleaseAll(void);

u8 *SlashEffect_GetOrCreateManager(void)
{
    u8 *p;
    if (gPursuingActorManager != 0)
        return gPursuingActorManager;
    p = Task_Create(0xa, 0xe1c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0812BFBC, PursuingActors_ReleaseAll);
        if (SlashEffect_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
