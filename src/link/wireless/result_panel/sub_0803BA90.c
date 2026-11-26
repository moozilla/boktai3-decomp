#include "global.h"

u32 ScenePool_PackResourceKey(u32 a, u32 b)
{
    u32 m = 0xFFFF;
    return ((a >> 24) << 16) | (b & m);
}
