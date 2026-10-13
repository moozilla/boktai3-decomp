#include "global.h"
extern u8 *gUnk_0200048C;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08050F40(u8 *, u32, u32, u32);
void Task_Kill(u8 *);
void sub_08050DBC(void);
void sub_08050E90(void);
u8 *sub_08051000(u32 a, u32 b, u32 c)
{
    u8 *r = gUnk_0200048C;
    if (r == 0) {
        r = Task_Create(9, 0x38);
        if (r) {
            Task_SetCallbacks(r, sub_08050DBC, sub_08050E90);
            if (sub_08050F40(r, a, b, c) < 0) {
                Task_Kill(r);
                return 0;
            }
        }
    }
    return r;
}
