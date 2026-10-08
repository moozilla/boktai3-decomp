#include "global.h"
void sub_08214514(void *);
void sub_08113684(u8 *s)
{
    u8 *p = s + 0xce;
    if (*p != 0) {
        u8 *q;
        u32 z;
        sub_08214514(s + 0x60);
        q = s + 0xcf;
        z = 0;
        *q = z;
        q -= 4;
        *q = z;
        *(u32 *)s = z;
        *p = z;
    }
}
