#include "contact_swarm_sprite.h"

struct ArcProjectile {
    u8 unknown0[4];
    u32 flags;
    u8 unknown8[0x18 - 8];
    u16 configFlags, duration;
    u8 unknown1C[0x8C - 0x1C];
    struct ContactSwarmSprite sprite;
    u8 unknownB8[0x17E - 0xB8];
    s16 limit;
};

void Projectile_SetUpdateCallback(void *, void (*)(void));
void TimedEffect_UpdateDeceleration(void);

static inline void ClearFlags(u32 *flags, u32 mask)
{
    *flags = mask & *flags;
}

s32 Projectile_InitArcFlight(struct ArcProjectile *projectile)
{
    Projectile_SetUpdateCallback(&projectile->configFlags, TimedEffect_UpdateDeceleration);
    {
        struct ContactSwarmSprite *sprite = &projectile->sprite;
        sprite->frame = 25;
    }
    ClearFlags(&projectile->flags, ~1);
    projectile->limit = projectile->sprite.position.y;
    return 0;
}
