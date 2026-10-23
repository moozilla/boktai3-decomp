// MATCHING_CONCESSION: register-pinning
#include "global.h"
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
#define GameOver_UpdateLetterTransforms sub_08163020
void GameOver_UpdateLetterTransforms(struct GameOver *p)
{
    s32 i;
    register s32 off asm("r1");
    for (i = 0; i <= 7; i++) {
        p->letters[i].scaleX = p->scale;
        p->letters[i].scaleY = p->scaleY;
    }
    i = 3;
    off = 0;
    for (; i >= 0; i--) {
        if (p->scale > 0x40) {
            p->letters[i].x = p->letters[i].baseX - off;
            {
                s32 t;
                off -= 16;
                t = p->scale;
                off += t >> 2;
            }
        } else
            p->letters[i].x = p->letters[i].baseX;
    }
    off = 0;
    for (i = 4; i <= 7; i++) {
        if (p->scale > 0x40) {
            p->letters[i].x = p->letters[i].baseX + off;
            {
                s32 t;
                off -= 16;
                t = p->scale;
                off += t >> 2;
            }
        } else
            p->letters[i].x = p->letters[i].baseX;
    }
}
