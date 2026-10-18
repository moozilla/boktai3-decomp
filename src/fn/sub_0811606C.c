#include "global.h"

extern u8 *gUnk_020001E8;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 ActorParticles_InitCompactPool(u8 *);
void Task_Kill(u8 *);
void sub_08115F74(void);
void CallbackPool_RemoveSmallSprites(void);

u8 *SpriteDrop_CreateManager(void)
{
    u8 *p;
    if (gUnk_020001E8 != 0)
        return gUnk_020001E8;
    p = Task_Create(0x8, 0xb98);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08115F74, CallbackPool_RemoveSmallSprites);
        if (ActorParticles_InitCompactPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
