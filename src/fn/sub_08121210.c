#include "global.h"

extern u8 *gUnk_0200020C;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 RegistryA8CB_InitManager(u8 *);
void Task_Kill(u8 *);
void sub_08121110(void);
void ActorManager_Destroy(void);

u8 *RegistryA8CB_EnsureManager(void)
{
    u8 *p;
    if (gUnk_0200020C != 0)
        return gUnk_0200020C;
    p = Task_Create(0x8, 0x920);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08121110, ActorManager_Destroy);
        if (RegistryA8CB_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
