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
    s16 scaleX;
    s16 scaleY;
};
#define GameOver_UpdateLetterTransforms sub_08163020
static inline s32 AddScaledLetterSpacing(s32 spread, struct GameOver *p)
{
    s32 scale = p->scaleX;
    return spread + (scale >> 2);
}

void GameOver_UpdateLetterTransforms(struct GameOver *p)
{
    s32 i;
    s32 spread;
    for (i = 0; i < 8; i++) {
        p->letters[i].scaleX = p->scaleX;
        p->letters[i].scaleY = p->scaleY;
    }
    i = 3;
    spread = 0;
    for (; i >= 0; i--) {
        if (p->scaleX > 0x40) {
            p->letters[i].x = p->letters[i].baseX - spread;
            spread = AddScaledLetterSpacing(spread - 16, p);
        } else
            p->letters[i].x = p->letters[i].baseX;
    }
    spread = 0;
    for (i = 4; i < 8; i++) {
        if (p->scaleX > 0x40) {
            p->letters[i].x = p->letters[i].baseX + spread;
            spread = AddScaledLetterSpacing(spread - 16, p);
        } else
            p->letters[i].x = p->letters[i].baseX;
    }
}
