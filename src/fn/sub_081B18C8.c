#include "paired_scene_actor.h"
#include "global.h"

// confidence=low: rises, waits for a countdown, then returns vertically
#include "display_animation.h"
#include "display_sequence_inline.h"
#include "motion_track.h"
void MotionTrack_Update(void *);
void Sound_PlaySong(u32);
static inline void StepSequence(struct PairedSceneHalf *owner)
{
    struct DisplaySequenceState *state;
    const u16 *command;
    state = &owner->graphics.sequence;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = owner->graphics.sequence.commands;
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

static inline void SetAnimation(struct PairedSceneHalf *p, struct VanargandAttackPair *boss, u32 id,
                                u32 options)
{
    void *data = (void *)boss->effectResource;
    struct DisplaySequenceState *seq;
    u32 playbackOptions;
    playbackOptions = options;
    seq = &p->graphics.sequence;
    Anim_SetSequence(seq, data, id, (u8)p->mode, playbackOptions);
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
static inline u8 MotionEnded(struct PairedSceneHalf *p)
{
    u8 result = 0;
    if (p->motion.active == 2)
        result = 1;
    return result;
}
static inline void SetState(struct PairedSceneHalf *p, u32 state)
{
    p->actionCounters[0] = state;
    p->actionCounters[1] = 0;
}
void ActorStatus_UpdateRisingStrike(struct VanargandAttackPair *boss, struct PairedSceneHalf *p)
{
    struct MotionTrackVector *targetPosition;
    struct MotionTrackVector rise;
    struct MotionTrackVector target;
    struct MotionTrackVector speed;
    u8 *enter;
    struct MotionTrack *motion;
    u32 mask = 0x20;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        SetAnimation(p, boss, 8, 4);
        PairedSceneHalf_BeginActionPhase(p, 0);
    }
    {
        u32 currentState = p->actionCounters[0];
        switch (currentState) {
            case 0: {
                u32 entered;
                {
                    u8 *address = &p->actionEntered;
                    entered = *address;
                    enter = address;
                }
                motion = &p->motion;
                if (entered) {
                    u32 zero = 0;
                    *enter = zero;
                    SetVector(&rise, zero, 512, zero);
                    {
                        struct MotionTrackVector *origin = (void *)&p->offset;
                        AddOffset(&rise, origin, &rise);
                        MotionTrack_Initialize(motion, origin, &rise, 0, 4);
                    }
                }
                MotionTrack_Update(motion);
                if (MotionEnded(p)) {
                    SetState(p, 1);
                    *enter = 1;
                }
                break;
            }

            case 1: {
                u32 entered;
                {
                    u8 *address = &p->actionEntered;
                    entered = *address;
                    enter = address;
                }
                if (entered) {
                    *enter = 0;
                    Sound_PlaySong(0x596);
                    p->shake = 60;
                }
                if (!p->shake) {
                    SetState(p, 2);
                    *enter = currentState;
                }
                break;
            }

            case 2: {
                u32 entered;
                {
                    u8 *address = &p->actionEntered;
                    entered = *address;
                    enter = address;
                }
                motion = &p->motion;
                if (entered) {
                    u32 zero = 0;
                    *enter = zero;
                    targetPosition = &target;
                    target = *(struct MotionTrackVector *)&p->offset;
                    targetPosition->y = -p->origin->y;
                    SetVector(&speed, zero, 64, zero);
                    MotionTrack_Initialize(motion, &p->offset, targetPosition, &speed, 1);
                }
                MotionTrack_Update(motion);
                if (MotionEnded(p)) {
                    SetState(p, 3);
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
