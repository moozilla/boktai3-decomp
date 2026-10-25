#include "global.h"
extern u8 *gUnk_020005EC;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SunOrb_InitManager(u8 *);
void Task_Kill(u8 *);
void OrbitEffect_UpdateManager(void);
void OrbitEffect_DestroyManager(void);
u8 *SunOrb_GetOrCreateManager(void)
{
    u8 *r;
    if (gUnk_020005EC != 0)
        return gUnk_020005EC;
    r = Task_Create(10, 0xef8);
    if (r) {
        Task_SetCallbacks(r, OrbitEffect_UpdateManager, OrbitEffect_DestroyManager);
        if (SunOrb_InitManager(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
