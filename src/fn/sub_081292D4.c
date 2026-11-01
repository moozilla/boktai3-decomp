#include "global.h"
#include "ram.h"
#include "player_cue_sequence.h"
struct Vec {
    s16 x, y, z, pad;
};
struct FeatherSequence {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct Feather {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[4];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct Vec position;
    u8 unknown24[8];
    struct FeatherSequence sequence;
    u8 unknown3C[0x54];
    u16 age;
    u8 state;
    s8 slot;
    u16 blinkAge;
    u8 enter, angle;
    u32 effectFlags, variant;
};
struct FeatherPool {
    u8 task[0x18];
    u8 graphics[0x1c];
    void *sequences;
    u32 used;
};
void Sprite_Remove(void *);
static inline void SelectDirection(u8 *angle, u8 *row, u8 *flip)
{
    u32 quadrant = ((*angle + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *flip = 1;
        *row = 3 - quadrant;
    } else {
        *flip = 0;
        *row = quadrant;
    }
}
static inline u32 Step(struct Feather *owner, struct FeatherSequence *state, struct Feather *base)
{
    return PlayerCue_StepSequenceEnded(owner, &base->sequence);
}

void FeatherEffectPool_SpawnGroundScatter(struct Feather *, s32);
// confidence=low: emits ground scatter on frame three, then releases the completed feather effect.
void FeatherEffectPool_UpdateScatterAnimation(void *unused, struct Feather *p)
{
    struct Feather *sprite = p;
    if (p->enter) {
        p->flags &= ~1;
        p->enter = 0;
    }
    if (p->sequence.index == 3 && !p->sequence.elapsed)
        FeatherEffectPool_SpawnGroundScatter(p, 4);
    if (Step(sprite, &p->sequence, p)) {
        struct FeatherPool *pool = gFadingSpritePool;
        pool->used &= ~(1 << p->slot);
        p->state = 0;
        Sprite_Remove(p);
        p->slot = -1;
        p->age = 0;
        p->enter = 1;
    }
}
