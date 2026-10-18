#include "global.h"
struct E {
    u16 a;
    u16 b;
    u8 c;
    u8 d;
    u16 e;
};
u32 Path_GetPointNodeId(struct E *t, u32 i)
{
    return t[i].c;
}
