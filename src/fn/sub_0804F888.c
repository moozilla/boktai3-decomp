#include "global.h"
extern u8 *gUnk_02000710;
void sub_0804F7D0(u8 *, u32, void (*)(void));
void sub_0804F91C(void);
void sub_0804F888(u8 *p)
{
    if (*(s16 *)(gUnk_02000710 + 0x876) > 0) {
        u16 *c = (u16 *)(p + 0xf4);
        u32 v = *c + 1;
        *c = v;
        if ((s32)(v << 16) > 0x012B0000)
            sub_0804F7D0(p, 0, sub_0804F91C);
    }
}
