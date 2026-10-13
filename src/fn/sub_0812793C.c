#include "global.h"

extern u8 *gUnk_02000164;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08127924(u8 *);
void Task_Kill(u8 *);
void sub_08127848(void);
void sub_081278E0(void);

u8 *sub_0812793C(void)
{
    u8 *p;
    if (gUnk_02000164 != 0)
        return gUnk_02000164;
    p = Task_Create(0x8, 0x864);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08127848, sub_081278E0);
        if (sub_08127924(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
