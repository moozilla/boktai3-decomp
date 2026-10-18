#include "global.h"
u32 Script_SeekToKeyword(u32);
void *Script_GetPc(void);
s32 Script_GetValue(void);
u32 Script_GetKeywordValue(u32, u32);
void sub_0822B6C8(s16, s16, s16, s16, u8, u16);
void sub_0822B968(void)
{
    u16 a;
    u16 b;
    u16 c;
    u8 f;
    u32 zero;
    if (Script_SeekToKeyword('p')) {
        a = Script_GetPc() ? Script_GetValue() : 0;
        b = Script_GetPc() ? Script_GetValue() : 0;
    } else {
        a = 0;
        b = 0;
    }
    c = Script_GetKeywordValue('r', 0);
    f = Script_GetKeywordValue('f', 1);
    zero = 0;
    sub_0822B6C8(a, b, c, zero, f, zero);
}
