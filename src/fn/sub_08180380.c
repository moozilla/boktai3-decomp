#include "global.h"
u8 *Task_Create(u32, u32);
void Script_GetKeywordValue(u32, u32);
u8 *sub_0802140C(void);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0817FDE0(u8 *, u32);
void Task_Kill(u8 *);
void sub_0817FD50(void);
void sub_0817FDA0(void);
u8 *sub_08180380(u32 a)
{
    u8 *p;
    Script_GetKeywordValue(0x74, 2);
    p = sub_0802140C();
    if (p != 0)
        return p;
    p = Task_Create(9, 0xC68);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0817FD50, sub_0817FDA0);
        if (sub_0817FDE0(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
