#include "global.h"

extern u8 *gUnk_02000580;
void sub_081029F8(u8 *);
void sub_08102C0C(u8 *);
s32 sub_081593D8(u8 *, u32);
void sub_08102EE8(u8 *);
s32 sub_08049F28(u32);
void sub_08102FB0(u8 *);
void sub_08219DD8(u8 *, u32);

void sub_08102E98(u8 *s)
{
    if (gUnk_02000580 != 0) {
        sub_081029F8(s);
        sub_08102C0C(s);
        if (sub_081593D8(gUnk_02000580, 1))
            sub_08102EE8(s);
        if (sub_08049F28(1))
            sub_08102FB0(s);
        sub_08219DD8(s + 0x58, 8);
    }
}
