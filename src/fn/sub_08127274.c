#include "global.h"
#include "ram.h"
#include "engine_globals.h"
#include "player_cue_sequence.h"
struct ParticleVector {
    u16 x, y, z, unused;
};
struct ParticleSequence {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct Projectile {
    u32 flags;
    u8 unknown4[2], angle, unknown7[9];
    u16 frame;
    u8 unknown12[10];
    struct ParticleVector position;
    u8 unknown24[8];
    struct ParticleSequence sequence;
    u8 unknown3C[0x1C];
    u8 collider[0x58];
    struct ParticleVector velocity;
    u8 unknownB8;
    s8 spin;
    u8 spinScale, colliderFlags;
    u16 timer, state;
    u8 active;
    s8 slot;
    u8 unknownC2[2];
    u32 resourceId;
    u8 enter, soundDelay;
    u16 lifetime, cell;
};
struct ProjectilePool {
    u8 unknown0[0x860];
    u32 allocated;
};
struct TerrainMap {
    u32 unknown0;
    u8 *cells;
    u8 unknown8[0x1C];
    u16 rows[1];
};
void *TerrainModifiers_FindCellOverride(u32, u32);
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
static inline s32 Cell(struct ParticleVector *p)
{
    s32 x = (s16)p->x >> 8, z = (s16)p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct TerrainMap *)gTerrainMap)->rows[z] + x;
}
static inline void Release(struct Projectile *p)
{
    struct ProjectilePool *pool = gPooledSpriteManager;
    s32 slot = p->slot;
    pool->allocated &= ~(1 << slot);
    p->active = 0;
    Sprite_Remove(p);
    p->slot = -1;
    p->state = 0;
    p->lifetime = 0;
}
// confidence=low: stationary projectile state expires on animation completion or lifetime.
void SpriteProjectile_UpdateExpiry(void *unused, struct Projectile *input)
{
    struct Projectile *p = input;
    struct Projectile *sprite = input;
    u32 ended;
    if (p->enter) {
        p->flags &= ~1;
        p->enter = 0;
    }
    p->cell = Cell(&p->position);
    ended = PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)sprite,
                                        (struct DisplaySequenceState *)&p->sequence);
    if (p->colliderFlags & 1) {
        void *collider = p->collider;
        Collider_SetPosition(collider, &p->position, 0);
        Collider_Refresh(collider);
    }
    if (!p->lifetime || ended) {
        Release(p);
        p->enter = 1;
    } else
        p->lifetime--;
}
