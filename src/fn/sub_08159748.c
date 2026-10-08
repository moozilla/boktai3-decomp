#include "global.h"
void sub_08159524(u8 *, u32);
static inline u32 tst(u32 *a, u32 m) { return *a & m; }
void sub_08159748(u8 *p, u32 n)
{
    if (p != 0 && tst((u32 *)(p + 0x444), 0x1000))
        sub_08159524(p, n << 1);
    else
        sub_08159524(p, n);
}
