#include "global.h"
u8 *sub_08055678(void);
s32 Script_SeekToKeyword(u32);
u32 Script_GetValue(void);
void sub_0821980C(u8 *, u8 *, u32, u32);
void sub_080556D0(void)
{
    u8 *p = sub_08055678();
    if (p) {
        if (Script_SeekToKeyword(0x70)) {
            u8 *a = p + 0x38;
            u8 *b = p + 0x18;
            sub_0821980C(a, b, (u16)Script_GetValue(), 0);
        }
    }
}
