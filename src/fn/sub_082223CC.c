#include "global.h"

u32 Heap_Alloc(u32);
void Memory_Clear(u32, u32);

s32 sub_082223CC(u32 *p, u32 n, u32 c)
{
    u32 sz;
    s32 r;
    p[0] = n;
    sz = n << 2;
    p[1] = Heap_Alloc(sz);
    if (p[1] != 0) {
        Memory_Clear(p[1], sz);
        p[2] = c;
        r = 0;
    } else {
        r = -1;
    }
    return r;
}
