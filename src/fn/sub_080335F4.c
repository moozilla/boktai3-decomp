#include "global.h"

struct S { u8 f[0x14]; u32 a; };
u32 Script_ParseStringRef(u32);
u32 Text_LookupString(u32);
u32 sub_080335F4(struct S *p, u32 i)
{
    return Text_LookupString(Script_ParseStringRef(p->a) + i);
}
