#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Z { struct H a, b; };
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_080560C0(struct Z *, s32, s32, s32);
void sub_080561E8(void)
{
    struct Z z;
    s32 a, b, c;
    if (Script_SeekToKeyword(0x70)) {
        z.a.lo = Script_GetValue();
        z.a.hi = Script_GetValue();
        z.b.lo = Script_GetValue();
    } else {
        u32 m = 0xFFFF0000;
        *(u32 *)&z.a = 0;
        *(u32 *)&z.b &= m;
    }
    if (Script_SeekToKeyword(0x66)) a = Script_GetValue();
    else a = 0;
    if (Script_SeekToKeyword(0x64)) b = Script_GetValue();
    else b = 0;
    if (Script_SeekToKeyword(0x74)) c = Script_GetValue();
    else c = 0;
    sub_080560C0(&z, ((b >> 1) + 1) & 3, c, a);
}
