#include "global.h"
void sub_081826C8(u8 *, u32, u32);
void SceneActor_SpawnSpriteEffect(u8 *);
void Armor_UpdateWalkFastWithTrailEffect(u8 *p)
{
    sub_081826C8(p, 15, 0x2A);
    if ((p[0xB42] & 0xF) == 0)
        SceneActor_SpawnSpriteEffect(p);
}
