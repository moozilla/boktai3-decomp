#include "global.h"
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
    u8 unknown4[0xC];
    u16 frame;
    u8 unknown12[10];
    struct ParticleVector position;
    u8 unknown24[8];
    struct ParticleSequence sequence;
    struct ParticleVector velocity;
    u8 collider[0x56];
    u8 active;
    s8 slot;
    u16 age;
    u8 enter, angle;
};
struct TerrainMap {
    u32 unknown0;
    u8 *cells;
    u8 unknown8[0x1C];
    u16 rows[1];
};
void *TerrainModifiers_FindCellOverride(u32, u32);
struct TimedSpritePool {
    u8 unknown0[0x34];
    void *resource;
};
void TimedSprite_BlinkAndRemove(struct TimedSpritePool *, struct Projectile *);
static inline u8 *SelectDirection(u8 *angle, u8 *row, u8 *flip)
{
    u8 *f;
    u32 quadrant;
    quadrant = ((*angle + 0x60) >> 6) & 3;
    f = flip;
    if (quadrant > 1) {
        *flip = 1;
        *row = 3 - quadrant;
    } else {
        *flip = 0;
        *row = quadrant;
    }
    return f;
}
// confidence=low: initializes direction-dependent playback before the blinking removal state.
void TimedSprite_UpdateBlinkAnimation(struct TimedSpritePool *pool, struct Projectile *input)
{
    struct Projectile *const p = input;
    u8 direction, mirrored;
    struct Projectile *incoming = input;
    if (p->enter) {
        u8 *flip = SelectDirection(&incoming->angle, &direction, &mirrored);
        Anim_SetSequence(&p->sequence, pool->resource, 19, direction, *flip);
        p->age = 0;
        p->enter = 0;
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)p,
                           (struct DisplaySequenceState *)&p->sequence);
    TimedSprite_BlinkAndRemove(pool, p);
    {
        u16 *const age = &p->age;
        (*age)++;
    }
}
