#include "global.h"

struct E {
    u16 a, b, c, f;
};
struct G {
    u32 a, g;
};

extern u32 gUnk_0824ED78[];
extern u32 gUnk_0824ED88[];
void sub_0818253C(u32 a, u32 b);
void Sound_PlaySongChecked(u32 a);

void sub_0818399C(u8 *p)
{
    u8 *s = p + 0xB40;
    u8 st = *s;

    switch (st) {
        case 0: {
            u32 four = 4;
            sub_0818253C((u32)p, (u32)&gUnk_0824ED78[((p[0xB39] + 0x60) >> 6) & 3]);
            (*s)++;
            p[0xB41] = st;
            ((struct E *)(p + 0x238))->f &= ~four;
            ((struct E *)(p + 0x1E4))->f &= ~four;
            ((struct E *)(p + 0x28C))->f &= ~four;
            ((struct E *)(p + 0x190))->f |= four;
            {
                u32 one = 1;
                ((struct G *)(p + 0x180))->g &= ~one;
            }
            *(u16 *)(p + 0xB32) &= ~0x20;
            Sound_PlaySongChecked(0xC1);
            break;
        }
        case 1:
            if (p[0xB43] != 0) {
                sub_0818253C((u32)p, (u32)&gUnk_0824ED88[((p[0xB39] + 0x60) >> 6) & 3]);
                *s = 0xFF;
                p[0xB41] = st;
            }
            break;
    }
}
