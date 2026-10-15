#include "global.h"
/* Callers also forward a command table and index; this implementation ignores them. */
void sub_0812E084(u8 *p, const void *table, u32 index)
{
    u32 z = 0;
    p[8] = z;
    p[9] = z;
    p[10] = z;
    *(u32 *)(p + 0xc) = z;
    *(u32 *)(p + 0x10) = z;
}
