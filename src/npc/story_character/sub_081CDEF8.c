#include "global.h"
extern s16 gUnk_086149C4[];
s32 Actor_PlayAnimationEntry(void *actor, s32 id, s32 a, s32 b, s32 c);
void sub_081CDEF8(u8 *a, u8 *b)
{
    s32 v;
    s32 off;
    s32 id;
    if (*(s16 *)(b + 0x26) == 1) {
        {
            s32 bit = 0x400;
            *(u16 *)(b + 0xe) |= bit;
        }
        v = gUnk_086149C4[(*(s32 *)(b + 0x10) << 2) & 0xff] << 4;
        if (v >= 0)
            off = v >> 12;
        else
            off = -((-v) >> 12);
        (*(u16 **)(b + 0x160))[1] += 0x80, (*(u16 **)(b + 0x160))[1] += off;
        if (b[1] == 0x1d && b[4] == 1) {
            switch (b[1]) {
                case 4:
                    id = 0;
                    break;
                case 9:
                    id = 0;
                    break;
                case 0x10:
                    id = 0;
                    break;
                case 0x14:
                    id = 0;
                    break;
                case 0x18:
                    id = 0;
                    break;
                case 0x1d:
                    id = 0xb;
                    break;
                default:
                    id = 0;
                    break;
            }
            Actor_PlayAnimationEntry(b, id, 1, 0, 1);
        }
    } else if (b[4] == 0x54) {
        s32 bit = 0x400;
        *(u16 *)(b + 0xe) |= bit;
    }
}
