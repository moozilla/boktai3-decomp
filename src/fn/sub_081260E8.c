#include "global.h"

extern u8 *gUnk_02000150;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_081260BC(u8 *);
void Task_Kill(u8 *);
void sub_08126000(void);
void sub_08126074(void);

u8 *sub_081260E8(void)
{
    u8 *p;
    if (gUnk_02000150 != 0)
        return gUnk_02000150;
    p = Task_Create(0xa, 0x6a0);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08126000, sub_08126074);
        if (sub_081260BC(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
