#include "global.h"
struct BF { u32 a : 16; u32 b : 16; u32 c : 16; };
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u32 sub_0821ABA8(u32, u32);
u32 sub_0800899C(struct BF *, u32, u32);

u32 sub_08008A00(void)
{
    struct BF bf;
    u32 x = sub_0821ABA8(0x6b, 0);
    u32 y = sub_0821ABA8(0x69, 0);
    if (Script_SeekToKeyword(0x70)) {
        bf.a = Script_GetValue();
        bf.b = Script_GetValue();
        bf.c = Script_GetValue();
    }
    return sub_0800899C(&bf, x, y);
}
