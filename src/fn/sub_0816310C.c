#include "global.h"

struct P {
    u8 f00[0x18];
    u8 a[8][0x50];
    u8 f298[0x2B8 - 0x298 - 0x0];
};

void Sprite_LinkToList(u8 *, s32);
void Anim_LinkToList(u8 *, s32);

void GameOver_ShowGraphics(u8 *p)
{
    u8 *q = p + 0x18;
    s32 i = 7;
    do {
        Sprite_LinkToList(q, 0);
        q += 0x50;
        i--;
    } while (i >= 0);
    Anim_LinkToList(p + 0x2B8, 0);
    *(u8 *)(p + 0x342) = 1;
}
