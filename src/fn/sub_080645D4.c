#include "global.h"

#ifdef NONMATCHING
// fuzzy: 97.3%

extern u8 *gUnk_02000710;
void Sound_PlaySong(u32);
void sub_08077848(u8 *, u32);

static inline u8 same(u16 *q)
{
    if (*(u32 *)(gUnk_02000710 + 0x5a0) == *q)
        return TRUE;
    return FALSE;
}

void sub_080645D4(u8 *p)
{
    u8 *e = *(u8 **)(p + 0x5c);
    u8 *f;
    u32 t;
    u32 one;
    if (e == NULL)
        return;
    if (!same((u16 *)(e + 0x2fa)))
        return;
    f = p + 0x32;
    one = 1;
    if (*f & 1)
        return;
    if (e[0x2b5] == 0x15)
        return;
    switch (e[0x98]) {
        case 4:
            Sound_PlaySong(0xee);
            goto done;
        case 1:
            if (p[0x3f] == 3)
                return;
            Sound_PlaySong(0xff);
            goto done;
        case 3:
            sub_08077848(e, 3);
        done:
            break;
        default:
            return;
    }
    t = *f;
    t |= one;
    *f = t;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080645D4);
#endif
