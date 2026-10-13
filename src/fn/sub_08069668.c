#include "global.h"

extern u8 *gUnk_0200014C;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08069620(u8 *);
void Task_Kill(u8 *);
void sub_08069510(void);
void sub_080695C8(void);

u8 *sub_08069668(void)
{
    u8 *p;
    if (gUnk_0200014C != 0)
        return gUnk_0200014C;
    p = Task_Create(0x8, 0xfd0);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08069510, sub_080695C8);
        if (sub_08069620(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
