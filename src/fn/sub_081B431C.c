#include "global.h"
#ifdef NONMATCHING
// fuzzy: 99.8%
// confidence=low: action role inferred from local animation, motion and effects.
#include "contact_swarm.h"
#include "motion_track.h"
#include "display_sequence_inline.h"
// Remaining difference: the burst count is materialized before the particle type load.

struct VanargandOwner {
    u8 unknown0[0x205];
    u8 firstAction;
    u8 unknown206[0x421 - 0x206];
    u8 secondAction;
    u8 unknown422[0x4D0 - 0x422];
    void *sequenceResource;
};

struct VanargandPart {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x1c];
    struct DisplaySequenceState sequence;
    struct MotionTrack motion;
    struct MotionTrackVector *basePosition;
    u8 unknown94[8];
    struct MotionTrackVector origin;
    struct MotionTrackVector target;
    s16 heading, distance, particleAngle;
    u8 unknownB2[0xaa];
    u32 variant, flags, state, timer;
    u8 currentAction, previousAction, reset, enter, finished;
    u8 unknown171[7];
    u32 actionTimer;
    u8 unknown17C[0x20];
    u32 flashTimer;
};
void MotionTrack_Update(void *);
extern const s16 gUnk_086149C4[];
static inline u32 StepSequence(struct VanargandPart *owner)
{
    struct DisplaySequenceState *state;
    const u16 *command;
    u32 ended;
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
                ended = 1;
            } else {
                state->index--;
                ended = 0;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
                ended = 1;
            } else {
                ended = 0;
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    } else {
        ended = 0;
    }
    return ended;
}

// confidence=low: paired part travels to an impact point and emits a burst.
s32 ScenePair_CountPositiveCounters(void *);
void Sound_PlaySong(u32);
void AnimatedParticles_EmitBurst(void *, s32, s32, s32, s32, s32, s32, s32);
void Vanargand_EmitPartTrail(void *);
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void Set(struct MotionTrackVector *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
static inline void Diff(struct MotionTrackVector *v, struct MotionTrackVector *a,
                        struct MotionTrackVector *b)
{
    v->x = a->x - b->x;
    v->y = a->y - b->y;
    v->z = a->z - b->z;
}
static inline void Vector(struct MotionTrackVector *v, s32 angle, s32 scale)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * scale);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * scale);
}
static inline void Add(struct MotionTrackVector *v, struct MotionTrackVector *b,
                       struct MotionTrackVector *a)
{
    v->x = b->x + a->x;
    v->y = b->y + a->y;
    v->z = b->z + a->z;
}
static inline u8 Ended(struct VanargandPart *p)
{
    return p->motion.active == 2;
}
static inline void SetState(struct VanargandPart *p, u32 state)
{
    p->state = state;
    p->timer = 0;
}
void Vanargand_UpdatePartImpact(struct VanargandOwner *boss, struct VanargandPart *p, u32 frame)
{
    u8 *enter;
    u32 entering;
    u32 savedFrame = frame;
    u32 mask = 0x20;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        p->state = 0;
        p->timer = 0;
        p->enter = 1;
    }
    switch (p->state) {
        case 0:
            entering = p->enter;
            enter = &p->enter;
            if (entering) {
                {
                    u32 zero = 0;
                    *enter = zero;
                    Anim_SetSequence(&p->sequence, boss->sequenceResource, 8, (u8)p->variant, zero);
                }
            }
            if (StepSequence(p)) {
                SetState(p, 1);
                *enter = 1;
            }
            break;

        case 1: {
            struct MotionTrackVector delta;
            entering = p->enter;
            enter = &p->enter;
            if (entering) {
                *enter = 0;
                if (p->variant == 1)
                    Vector(&delta, 224, 64);
                else
                    Vector(&delta, 96, 64);
                Add(&delta, &delta, &p->origin);
                MotionTrack_Initialize(&p->motion, &p->origin, &delta, 0, 4);
            }
            MotionTrack_Update(&p->motion);
            if (Ended(p)) {
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
                if (p->variant == 1) {
                    Set(&p->target, 0x3ab, 0x100, 0x2cb);
                } else {
                    Set(&p->target, 0x2cb, 0x100, 0x3ab);
                }
                Diff(&p->target, &p->target, p->basePosition);
                MotionTrack_Initialize(&p->motion, &p->origin, &p->target, 0, 2);
            }
            MotionTrack_Update(&p->motion);
            if (Ended(p)) {
                if ((ScenePair_CountPositiveCounters(boss) == 2) &&
                    (*((p->variant == 1) ? &boss->firstAction : &boss->secondAction) == 12)) {
                    struct MotionTrackVector point;
                    Sound_PlaySong(0x1ba);
                    Add(&point, &p->target, p->basePosition);
                    AnimatedParticles_EmitBurst(&point, p->particleAngle, 0, 64, 4, 4, 4, 4);
                    p->flashTimer = 15;
                    SetState(p, 3);
                } else {
                    void *position;
                    s32 vertical;
                    Sound_PlaySong(0x3d9);
                    position = &p->sprite.position;
                    vertical = 4;
                    AnimatedParticles_EmitBurst(position, p->particleAngle, 0, 64, vertical, 4, 4,
                                                4);
                    SetState(p, 3);
                }
                {
                    u32 one = 1;
                    *enter = one;
                    p->finished = one;
                }
            } else if (!(savedFrame & 1))
                Vanargand_EmitPartTrail(p);
            break;

        case 3:
            p->finished = 1;
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Vanargand_UpdatePartImpact);
#endif
