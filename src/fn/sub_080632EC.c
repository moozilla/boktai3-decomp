#include "global.h"

void Sprite_Remove(u8 *);
void Anim_Remove(u8 *);
void SceneInput_DestroyMarkerPair(u8 *);
void SceneInput_DestroyGlyphPair(u8 *);
void sub_08030BF8(void);
void sub_08033468(void);

s32 sub_080632EC(u8 *p)
{
    u8 *q;
    s32 i;
    Sprite_Remove(p + 0x60);
    q = p + 0xa8;
    i = 0x44;
    do {
        Anim_Remove(q);
        q += 0x60;
        i--;
    } while (i >= 0);
    SceneInput_DestroyMarkerPair(p + 0x1af0);
    SceneInput_DestroyGlyphPair(p + 0x1c14);
    sub_08030BF8();
    sub_08033468();
    return 0;
}
