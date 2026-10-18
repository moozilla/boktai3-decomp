#include "global.h"

extern u8 *gUnk_02000180;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 FragmentProjectilePool_Init(u8 *);
void Task_Kill(u8 *);
void ActorEffect_UpdatePool(void);
void sub_0812B27C(void);

u8 *FragmentProjectilePool_GetOrCreate(void)
{
    u8 *p;
    if (gUnk_02000180 != 0)
        return gUnk_02000180;
    p = Task_Create(0x8, 0xc80);
    if (p != 0) {
        Task_SetCallbacks(p, ActorEffect_UpdatePool, sub_0812B27C);
        if (FragmentProjectilePool_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
