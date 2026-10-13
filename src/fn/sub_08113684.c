#include "global.h"
void Sprite_Remove(void *);
void sub_08113684(u8 *s)
{
    u8 *p = s + 0xce;
    if (*p != 0) {
        u8 *q;
        u32 z;
        Sprite_Remove(s + 0x60);
        q = s + 0xcf;
        z = 0;
        *q = z;
        q -= 4;
        *q = z;
        *(u32 *)s = z;
        *p = z;
    }
}
