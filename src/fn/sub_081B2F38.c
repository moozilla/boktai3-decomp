#include "global.h"
#include "contact_swarm.h"
#include "motion_track.h"
#include "display_sequence_inline.h"

// confidence=low: inferred from adjacent Vanargand part callbacks and local state transitions.

struct VanargandPartOwner {
    u8 unknown0[0x4D0];
    void *sequenceResource;
};
struct VanargandPart {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x1c];
    struct DisplaySequenceState sequence;
    struct MotionTrack motion;
    struct MotionTrackVector *anchor;
    u8 unknown94[8];
    struct MotionTrackVector origin;
    struct MotionTrackVector target;
    s16 heading, distance, particleAngle;
    u8 unknownB2[0xaa];
    u32 variant, flags, state, timer;
    u8 unknown16C[2];
    u8 reset, enter, finished;
};
void OrbitMotion_BeginDistanceWave(void *, const void *, const void *, s32, s32);
void MotionTrack_Update(void *);
void Vanargand_EmitPartTrail(void *);
void Sound_PlaySong(u32);
void AnimatedParticles_EmitBurst(void *, s32, s32, s32, s32, s32, s32, s32);
struct MotionTrackVector *Player_GetEffectivePosition(u32);
s32 Math_Atan2(s32, s32);
extern const s16 gUnk_086149C4[];
static inline s32 TruncateShift(s32 x, u32 bits)
{
    return x >= 0 ? x >> bits : -((-x) >> bits);
}
static inline void MakeVector(struct MotionTrackVector *out, s32 angle, s32 scale)
{
    out->x = TruncateShift(gUnk_086149C4[(angle + 64) & 255] * scale, 12);
    out->y = 0;
    out->z = TruncateShift(gUnk_086149C4[angle & 255] * scale, 12);
}
static inline void AddVector(struct MotionTrackVector *dst, struct MotionTrackVector *a,
                             struct MotionTrackVector *b)
{
    dst->x = b->x + a->x;
    dst->y = b->y + a->y;
    dst->z = b->z + a->z;
}
static inline void Difference(struct MotionTrackVector *dst, struct MotionTrackVector *a,
                              struct MotionTrackVector *b)
{
    dst->x = a->x - b->x;
    dst->y = a->y - b->y;
    dst->z = a->z - b->z;
}
static inline s32 AimAngle(struct VanargandPart *p)
{
    s32 x = p->sprite.position.x;
    s32 z = p->sprite.position.z;
    s32 dx = Player_GetEffectivePosition(0)->x;
    s32 angle;
    {
        s32 dz = Player_GetEffectivePosition(0)->z;
        angle = Math_Atan2(dx - x, dz - z) & 255;
    }
    if (((u32)(angle - 16)) > 176)
        angle = 16;
    else if (angle > 112)
        angle = 112;
    return angle;
}

static inline void SetFlags(struct VanargandPart *p, u32 mask)
{
    p->flags |= mask;
}
static inline void SetState(struct VanargandPart *p, u32 state)
{
    p->state = state;
    p->timer = 0;
}
static inline u8 MotionEnded(struct VanargandPart *p)
{
    return p->motion.active == 2;
}
static inline void OffsetTarget(struct MotionTrackVector *v, u32 variant)
{
    if (variant == 0)
        v->x += 128;
    else
        v->z += 128;
}
// confidence=low: attacks a position offset from the player before returning.
static inline void StepSequence(struct VanargandPart *owner)
{
    struct DisplaySequenceState *state;
    const u16 *command;
    state = &owner->sequence;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = owner->sequence.commands;
        command = (const u16 *)((u32)table + offset);
    }
    owner->sprite.frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
            } else {
                state->index--;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    }
}

void Vanargand_UpdatePartOffsetStrike(struct VanargandPartOwner *boss, struct VanargandPart *p,
                                      u32 tick)
{
    u8 *enter;
    u32 entering;
    u32 mask = 0x20;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        p->state = 0;
        p->timer = 0;
        p->enter = 1;
    }
    switch (p->state) {
        case 0: {
            struct MotionTrackVector delta;
            u32 one;
            void *motion;
            SetFlags(p, 8);
            entering = p->enter;
            enter = &p->enter;
            if (entering) {
                *enter = 0;
                MakeVector(&delta, (AimAngle(p) + 128) & 255, 128);
                AddVector(&delta, &p->origin, &delta);
                motion = &p->motion;
                one = 1;
                MotionTrack_Initialize(motion, &p->origin, &delta, 0, one);
                {
                    void *data = boss->sequenceResource;
                    struct DisplaySequenceState *seq = &p->sequence;
                    Anim_SetSequence(seq, data, 7, (u8)p->variant, 4);
                    StepSequence(p);
                }
            }
            MotionTrack_Update(&p->motion);
            if (MotionEnded(p)) {
                SetState(p, 1);
                *enter = 1;
            }
            break;
        }

        case 1: {
            struct MotionTrackVector delta;
            entering = p->enter;
            enter = &p->enter;
            if (entering) {
                u32 zero = 0;
                *enter = zero;
                delta = *Player_GetEffectivePosition(zero);
                OffsetTarget(&delta, p->variant);
                Difference(&delta, &delta, p->anchor);
                MotionTrack_Initialize(&p->motion, &p->origin, &delta, 0, 2);
            }
            if ((++p->timer) > 20) {
                SetState(p, 2);
                *enter = 1;
            }
            break;
        }

        case 2:
            entering = p->enter;
            enter = &p->enter;
            if (entering) {
                *enter = 0;
                Sound_PlaySong(0x589);
            }
            MotionTrack_Update(&p->motion);
            if (!(tick & 1))
                Vanargand_EmitPartTrail(p);
            if (MotionEnded(p)) {
                Sound_PlaySong(0x3d9);
                AnimatedParticles_EmitBurst(&p->sprite.position, p->particleAngle, 0, 64, 4, 4, 4,
                                            4);
                SetState(p, 3);
                *enter = 1;
            }
            break;

        case 3: {
            struct MotionTrackVector delta;
            SetFlags(p, 8);
            entering = p->enter;
            enter = &p->enter;
            if (entering) {
                *enter = 0;
                MakeVector(&delta, p->heading, p->distance);
                MotionTrack_Initialize(&p->motion, &p->origin, &delta, 0, 1);
            }
            MotionTrack_Update(&p->motion);
            if (MotionEnded(p)) {
                SetState(p, 4);
                *enter = 1;
            }
            break;
        }

        case 4:
            SetFlags(p, 8);
            p->finished = 1;
            break;
    }
}
