#include "global.h"
/* Show the paired foreground objects and their associated objects. */
struct Obj {
    u32 a, b, flags;
};
void sub_081EAAAC(u8 *p)
{
    ((struct Obj *)(p + 0x238))->flags &= ~1;
    ((struct Obj *)(p + 0x2c0))->flags &= ~1;
    *(u32 *)(p + 0x350) &= ~1;
    *(u32 *)(p + 0x3d8) &= ~1;
}
