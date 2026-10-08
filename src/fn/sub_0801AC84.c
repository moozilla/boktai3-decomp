#include "global.h"
void sub_0801ABA0(u8 *);
void sub_0801A5A8(u8 *);
void sub_0801A814(u8 *);
s32 Script_ParseStringRef(s32);
s32 Text_LookupString(void);
void sub_0801A01C(u8 *, s32);
void sub_0801AC84(u8 *p)
{
    sub_0801ABA0(p);
    sub_0801A5A8(p);
    sub_0801A814(p);
    Script_ParseStringRef(*(s32 *)(p + 0xDB0));
    sub_0801A01C(p, Text_LookupString());
}
