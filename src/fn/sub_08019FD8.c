#include "global.h"
u32 Script_ParseStringRef(u32);
u32 Text_LookupString(u32);
void sub_08030C84(u32);
void sub_08030DBC(u32);
void sub_08033478(u32, u32);
void sub_0803343C(u32);
void sub_08019FD8(u8 *p, u32 q)
{
    u32 s = Script_ParseStringRef(q) + 0x30;
    sub_08030C84(q);
    sub_08030DBC(*p);
    sub_08033478(0, 0);
    sub_08033478(1, 999);
    sub_0803343C(Text_LookupString(s + *p));
}
