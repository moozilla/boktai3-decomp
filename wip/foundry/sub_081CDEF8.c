#include "global.h"
extern const s16 gUnk_086149C4[];
s32 Actor_PlayAnimationEntry(void *actor, s32 id, s32 a, s32 b, s32 c);

static inline s32 trunc12(s32 x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}

void sub_081CDEF8(u8 *a, u8 *b)
{
    u32 id;
    s16 mode = *(s16 *)(b + 0x26);

    if (mode == 1) {
        *(u16 *)(b + 0xe) |= 0x400;
        {
            u32 angle = *(u32 *)(b + 0x10);
            s32 sample = gUnk_086149C4[(angle << 2) & 0xff];
            s32 delta = trunc12(sample << 4);
            u8 *obj = *(u8 **)(b + 0x160);
            *(u16 *)(obj + 2) += 0x80 + delta;
        }
        if (b[1] == 0x1d) {
          if (b[4] == 1) {
            switch (b[1]) {
            case 29:
                id = 0xb;
                break;
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            default:
                id = 0;
                break;
            }
            Actor_PlayAnimationEntry(b, id, 1, 0, 1);
          }
        }
    } else if (b[4] == 0x54) {
        *(u16 *)(b + 0xe) |= 0x400;
    }
}
