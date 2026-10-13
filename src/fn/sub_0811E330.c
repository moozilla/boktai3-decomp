#include "global.h"

extern u8 *gUnk_02000208;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0811E318(u8 *);
void Task_Kill(u8 *);
void sub_0811DEDC(void);
void sub_0811DEE0(void);

u8 *sub_0811E330(void)
{
    u8 *p;
    if (gUnk_02000208 != 0)
        return gUnk_02000208;
    p = Task_Create(8, 0x23c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0811DEDC, sub_0811DEE0);
        if (sub_0811E318(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
