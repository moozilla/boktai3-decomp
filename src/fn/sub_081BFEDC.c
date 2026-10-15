#include "global.h"
void Anim_Remove(u8 *);
void SceneInput_DestroyMarkerPair(u8 *);
void SceneInput_DestroyGlyphPair(u8 *);
void sub_081BFEDC(u8 *p)
{
    u8 *q = p + 0x18;
    s32 i = 0x37;
    do {
        if (q[4])
            Anim_Remove(q);
        q += 0x60;
        i--;
    } while (i >= 0);
    SceneInput_DestroyMarkerPair(p + 0x1558);
    SceneInput_DestroyGlyphPair(p + 0x164c);
}
