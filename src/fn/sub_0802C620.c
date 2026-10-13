#include "global.h"
u8 *Task_Create(u32, u32);
void Script_GetKeywordValue(u32, u32);
u8 *sub_0802140C(void);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0802BFAC(u8 *, u32);
void Task_Kill(u8 *);
void sub_0802BF30(void);
void sub_0802BF88(void);
u8 *sub_0802C620(u32 a)
{
    u8 *p;
    Script_GetKeywordValue(0x74, 3);
    p = sub_0802140C();
    if (p != 0)
        return p;
    p = Task_Create(8, 0x924);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0802BF30, sub_0802BF88);
        if (sub_0802BFAC(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
