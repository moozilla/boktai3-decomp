#include "global.h"
struct BF { u32 a : 16; u32 b : 16; u32 c : 16; };
u32 sub_0821ABA8(u32, u32);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 sub_08013EF4(u32, struct BF *);
s32 sub_08013F24(void)
{
    struct BF bf;
    u32 x = sub_0821ABA8(0x6e, 0);
    s32 r;
    if (Script_SeekToKeyword(0x70) == 0) {
        r = -1;
    } else {
        bf.a = Script_GetValue();
        bf.b = Script_GetValue();
        bf.c = Script_GetValue();
        r = sub_08013EF4(x, &bf);
    }
    return r;
}
