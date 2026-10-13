#include "global.h"

extern u8 *gUnk_020001A4;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08100038(u8 *);
void Task_Kill(u8 *);
void sub_080FFF78(void);
void sub_080FFFE4(void);

u8 *sub_08100068(void)
{
    u8 *p;
    if (gUnk_020001A4 != 0)
        return gUnk_020001A4;
    p = Task_Create(9, 0xa40);
    if (p != 0) {
        Task_SetCallbacks(p, sub_080FFF78, sub_080FFFE4);
        if (sub_08100038(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
