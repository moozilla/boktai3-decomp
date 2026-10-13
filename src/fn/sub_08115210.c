#include "global.h"

extern u8 *gUnk_020001DC;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_081151F8(u8 *);
void Task_Kill(u8 *);
void sub_08115104(void);
void sub_08115190(void);

u8 *sub_08115210(void)
{
    u8 *p;
    if (gUnk_020001DC != 0)
        return gUnk_020001DC;
    p = Task_Create(0xa, 0xaa0);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08115104, sub_08115190);
        if (sub_081151F8(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
