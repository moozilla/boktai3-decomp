#include "global.h"
struct BF { u32 a : 16; u32 b : 16; u32 c : 16; };
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u32 sub_0821ABA8(u32, u32);
void sub_08005E64(u32, struct BF *, u32, u32, u32);

u32 sub_080062C4(void)
{
    struct BF bf;
    u32 a = sub_0821ABA8(0x6e, 0);
    u32 b = sub_0821ABA8(0x74, 0);
    u32 c = sub_0821ABA8(0x66, 0);
    u32 d = sub_0821ABA8(0x65, 0);
    if (Script_SeekToKeyword(0x70)) {
        bf.a = Script_GetValue();
        bf.b = Script_GetValue();
        bf.c = Script_GetValue();
    } else {
        bf.a = 0;
        bf.b = 0;
        bf.c = 0;
    }
    sub_08005E64(a, &bf, c, b, d);
    return 0;
}
