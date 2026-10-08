#include "global.h"
void sub_0801AB1C(u8 *);
void sub_0801A53C(u8 *);
void sub_0801A75C(u8 *);
s32 Script_ParseStringRef(s32);
s32 Text_LookupString(void);
void sub_08019F44(u8 *, s32);
void sub_0801AC50(u8 *p)
{
    sub_0801AB1C(p);
    sub_0801A53C(p);
    sub_0801A75C(p);
    Script_ParseStringRef(*(s32 *)(p + 0xDB0));
    sub_08019F44(p, Text_LookupString());
}
