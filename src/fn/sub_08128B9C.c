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
void FeatherEffectPool_UpdateOpeningAnimation(struct FeatherPool *pool, struct Feather *p)
{
    struct Feather *sprite = p;
    u8 row, flip;
    if (p->enter) {
        p->flags &= ~1;
        p->enter = 0;
    }
    if (PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)sprite,
                                    (struct DisplaySequenceState *)&p->sequence)) {
        SelectDirection(&p->angle, &row, &flip);
        Anim_SetSequence(&p->sequence, pool->sequences, 1, row, flip);
        p->state = 2;
        p->enter = 1;
    }
}
