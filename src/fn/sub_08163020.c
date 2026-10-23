#include "global.h"
#ifdef NONMATCHING
// fuzzy: 96.2%
// Open: the ROM subtracts 0x10 from off before loading scale (subs r1,#0x10 first);
// writing off -= 0x10 separately fixes that order but swaps the off/pointer registers.
struct GameOverLetter {
    u8 scaleX, scaleY;
    u8 pad02[0x12];
    u16 x;
    u8 pad16[0x2a];
    u16 baseX;
    u8 pad42[0xe];
};
struct GameOver {
    u8 pad00[0x20];
    struct GameOverLetter letters[8];
    u8 pad2A0[0x9c];
    s16 scale;
    s16 scaleY;
};
void sub_08163020(struct GameOver *p)
{
    s32 i;
    s32 off;
    for (i = 0; i <= 7; i++) {
        p->letters[i].scaleX = p->scale;
        p->letters[i].scaleY = p->scaleY;
    }
    off = 0;
    for (i = 3; i >= 0; i--) {
        if (p->scale > 0x40) {
            p->letters[i].x = p->letters[i].baseX - off;
            {
                s32 t = p->scale;
                off += -0x10 + (t >> 2);
            }
        } else
            p->letters[i].x = p->letters[i].baseX;
    }
    off = 0;
    for (i = 4; i <= 7; i++) {
        if (p->scale > 0x40) {
            p->letters[i].x = p->letters[i].baseX + off;
            {
                s32 t = p->scale;
                off += -0x10 + (t >> 2);
            }
        } else
            p->letters[i].x = p->letters[i].baseX;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08163020);
#endif
