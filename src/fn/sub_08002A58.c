#include "global.h"

s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
u32 sub_0821ABA8(u32, u32);
void ScrollingBackground_QueueCommand(u32, u32, s32, s32 *);
void sub_08002A58(void)
{
    s32 buf[8];
    u32 a = sub_0821ABA8(0x74, 1);
    u32 b = sub_0821ABA8(0x72, 0);
    s32 n = 0;
    if (Script_SeekToKeyword(0x70)) {
        s32 *q = buf;
        while (Script_GetPc() != 0 && n <= 7) {
            *q++ = Script_GetValue();
            n++;
        }
    }
    ScrollingBackground_QueueCommand(a, b, n, buf);
}
