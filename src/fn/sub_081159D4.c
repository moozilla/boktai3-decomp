#include "global.h"

extern u8 *gUnk_020001E4;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08115924(u8 *);
void Task_Kill(u8 *);
void sub_081158B8(void);
void Collectible_DestroyManager(void);

u8 *Collectible_CreateManager(void)
{
    u8 *p;
    if (gUnk_020001E4 != 0)
        return gUnk_020001E4;
    p = Task_Create(0x8, 0x2e98);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081158B8, Collectible_DestroyManager);
        if (sub_08115924(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
