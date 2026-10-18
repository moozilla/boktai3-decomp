#include "global.h"
struct E {
    u16 a;
    u16 b;
    u16 c;
    u16 d;
};
u32 Path_GetPointFacing(struct E *t, u32 i)
{
    return t[i].c >> 8;
}
