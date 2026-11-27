#include "effects/projectiles/timed_effect_manager.h"
#include "scene/collision/contact_position.h"
#include "engine/engine.h"
#include "engine/ram.h"
extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline s32 Rand2(s32 i)
{
    gRandomIndex = (i + 1) & 1023;
    return gRandomTable[(i + 1) & 1023];
}
// confidence=low: starts the next trail sprite slot at a random offset around the owner direction.
void ProjectileTask_SpawnTrailSprite(struct TimedEffectManager *manager,
                                     struct Task4E01Projectile *source)
{
    u16 mul;
    s32 dir;
    s32 dir_2;
    s32 *np;
    s32 ri;
    const u16 *tab;
    int k2 = 0x100;
    manager->effects[manager->nextTrail].active = 1;
    manager->effects[manager->nextTrail].sprite.flags &= ~1;
    manager->effects[manager->nextTrail].timer = 0;
    np = &manager->nextTrail;
    tab = gRandomTable;
    ri = (gRandomIndex + 1) & 1023;
    dir_2 = (s32)tab[ri];
    manager->effects[manager->nextTrail].radius = ((dir_2 >> 3) & 0x7f) + 0x40;
    manager->effects[*np].angleOffset = Mod(Rand2(ri), 0x60) - 0x30;
    dir = (u8)((((u16)source->params.direction) + 0x70) >> 5) & 7;
    dir = ((dir + 5) & 7) << 5;
    dir = (dir + (manager->effects[manager->nextTrail].angleOffset + k2)) & 0xff;
    manager->effects[manager->nextTrail].sprite.position = source->sprite.position;
    mul = manager->effects[manager->nextTrail].radius;
    manager->effects[manager->nextTrail].sprite.position.x +=
        Truncate(gUnk_086149C4[(dir + 0x40) & 0xff] * mul);
    mul = manager->effects[manager->nextTrail].radius;
    manager->effects[manager->nextTrail].sprite.position.z += Truncate(mul * gUnk_086149C4[dir]);
    manager->nextTrail++;
    if (manager->nextTrail > 3)
        manager->nextTrail = 0;
}
