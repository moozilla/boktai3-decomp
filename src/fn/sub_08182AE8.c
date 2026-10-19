#include "global.h"
void Armor_UpdateRunMotion(u8 *, u32, u32);
void SceneActor_SpawnSpriteEffect(u8 *);
void Armor_UpdateRunWithTrailEffect(u8 *p)
{
    Armor_UpdateRunMotion(p, 20, 0x20);
    if ((p[0xB42] & 0x7) == 0)
        SceneActor_SpawnSpriteEffect(p);
}
