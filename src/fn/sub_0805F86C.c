#include "global.h"

struct S { u32 a[3]; u32 v; };
extern struct S gUnk_08E61280[];

u32 sub_0805F86C(s32 i)
{
    return gUnk_08E61280[i].v;
}
