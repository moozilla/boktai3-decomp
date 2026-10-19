#include "global.h"
void Armor_UpdateWalk(u8 *, u32, u32);
void SceneActor_SpawnSpriteEffect(u8 *);
void Armor_UpdateWalkFastWithTrailEffect(u8 *p)
{
    Armor_UpdateWalk(p, 15, 0x2A);
    if ((p[0xB42] & 0xF) == 0)
        SceneActor_SpawnSpriteEffect(p);
}
