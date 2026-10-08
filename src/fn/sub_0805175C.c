#include "global.h"
struct W { u32 a, b; };
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_082151E4(u8 *, s32);
void sub_082144A4(u8 *, u8 *, u32);
void sub_0805175C(u8 *p, struct W *w)
{
    u8 *q = p + 0x18;
    u8 *r = p + 0x44;
    s32 v;
    if (Script_SeekToKeyword(0x74))
        sub_082151E4(r, Script_GetValue());
    else
        sub_082151E4(r, 0x9D41);
    sub_082144A4(q, r, 0);
    v = Script_SeekToKeyword(0x69);
    if (v) v = Script_GetValue();
    *(u16 *)(q + 0x10) = v;
    *(struct W *)(q + 0x1c) = *w;
    if (Script_SeekToKeyword(0x72) && Script_GetValue())
        *(u32 *)q |= 4;
}
