#include "global.h"

// confidence=low: rises, waits for a countdown, then returns vertically
#include "display_animation.h"
#include "display_sequence_inline.h"
#include "motion_track.h"
struct ActorStatusOwner {
    u8 unknown0[0x4D0];
    void *sequenceResource;
};
struct ActorStatusPart {
    u32 spriteFlags;
    u8 unknown4[0xc];
    u16 frame;
    u8 unknown12[0xa];
    struct MotionTrackVector position;
    u8 unknown24[0x24];
    struct DisplaySequenceState sequence;
    struct MotionTrack motion;
    struct MotionTrackVector *anchor;
    u8 unknown94[8];
    struct MotionTrackVector offset;
    u8 unknownA4[0xb8];
    u32 variant, flags, state, timer;
    u8 action, previousAction, reset, enter, finished;
    u8 unknown171[0x2b];
    u32 countdown;
};
void MotionTrack_Update(void *);
void Sound_PlaySong(u32);
static inline void StepSequence(struct ActorStatusPart *owner)
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
    owner->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->spriteFlags |= 4;
        else
            owner->spriteFlags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->spriteFlags |= 8;
        else
            owner->spriteFlags &= ~8;
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

static inline void SetAnimation(struct ActorStatusPart *p, struct ActorStatusOwner *boss, u32 id,
                                u32 options)
{
    void *data = boss->sequenceResource;
    struct DisplaySequenceState *seq;
    u32 playbackOptions;
    playbackOptions = options;
    seq = &p->sequence;
    Anim_SetSequence(seq, data, id, (u8)p->variant, playbackOptions);
    StepSequence(p);
}

static inline void SetVector(struct MotionTrackVector *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}
static inline void AddOffset(struct MotionTrackVector *out, struct MotionTrackVector *a,
                             struct MotionTrackVector *b)
{
    out->x = (u16)b->x + (u16)a->x;
    out->y = (u16)a->y + (u16)b->y;
    out->z = (u16)b->z + (u16)a->z;
}
static inline u8 MotionEnded(struct ActorStatusPart *p)
{
    u8 result = 0;
    if (p->motion.active == 2)
        result = 1;
    return result;
}
static inline void SetState(struct ActorStatusPart *p, u32 state)
{
    p->state = state;
    p->timer = 0;
}
void ActorStatus_UpdateRisingStrike(struct ActorStatusOwner *boss, struct ActorStatusPart *p)
{
    struct MotionTrackVector *targetPosition;
    struct MotionTrackVector rise;
    struct MotionTrackVector target;
    struct MotionTrackVector speed;
    u8 *enter;
    struct MotionTrack *motion;
    u32 mask = 0x20;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        SetAnimation(p, boss, 8, 4);
        {
            u32 zero = 0;
            p->state = zero;
            p->timer = zero;
        }
        p->enter = 1;
    }
    {
        u32 currentState = p->state;
        switch (currentState) {
            case 0: {
                u32 entered;
                {
                    u8 *address = &p->enter;
                    entered = *address;
                    enter = address;
                }
                motion = &p->motion;
                if (entered) {
                    u32 zero = 0;
                    *enter = zero;
                    SetVector(&rise, zero, 512, zero);
                    {
                        struct MotionTrackVector *origin = &p->offset;
                        AddOffset(&rise, origin, &rise);
                        MotionTrack_Initialize(motion, origin, &rise, 0, 4);
                    }
                }
                MotionTrack_Update(motion);
                if (MotionEnded(p)) {
                    u32 next = 1;
                    SetState(p, next);
                    *enter = next;
                }
                break;
            }

            case 1: {
                u32 entered;
                {
                    u8 *address = &p->enter;
                    entered = *address;
                    enter = address;
                }
                if (entered) {
                    {
                        u8 k2 = 0;
                        *enter = k2;
                    }
                    Sound_PlaySong(0x596);
                    p->countdown = 60;
                }
                if (!p->countdown) {
                    {
                        u8 k1 = 2;
                        SetState(p, k1);
                    }
                    *enter = currentState;
                }
                break;
            }

            case 2: {
                u32 entered;
                {
                    u8 *address = &p->enter;
                    entered = *address;
                    enter = address;
                }
                motion = &p->motion;
                if (entered) {
                    u32 zero = 0;
                    *enter = zero;
                    targetPosition = &target;
                    target = p->offset;
                    targetPosition->y = -p->anchor->y;
                    SetVector(&speed, zero, 64, zero);
                    MotionTrack_Initialize(motion, &p->offset, targetPosition, &speed, 1);
                }
                MotionTrack_Update(motion);
                if (MotionEnded(p)) {
                    {
                        u32 next = 3;
                        SetState(p, next);
                    }
                    *enter = 1;
                }
                break;
            }

            case 3:
                p->finished = 1;
                break;
        }
    }
}
