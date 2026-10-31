#include "global.h"
#include "display_animation.h"
#include "display_sequence_inline.h"
#include "player.h"
#include "motion_track.h"
// confidence=low: moves a paired half toward the player, then holds and releases the position request.
struct PairHalf {
    u32 flags;
    u8 pad4[0x10 - 4];
    u16 frame;
    u8 pad12[0x1C - 0x12];
    struct MotionTrackVector position;
    u8 pad24[0x48 - 0x24];
    struct DisplaySequenceState sequence;
    struct MotionTrack track;
    struct MotionTrackVector *origin;
    u8 pad94[8];
    struct MotionTrackVector offset;
    u8 padA4[0x15C - 0xA4];
    u32 mode, controlFlags;
    u32 phase, timer;
    u8 state, previousState, entered, phaseEntered, finished;
};
struct PairOwner {
    u8 pad0[0x4D0];
    void *resource;
};
struct CapturePlayer {
    u8 pad0[0x20];
    u32 flags;
};
void MotionTrack_Update(void *);
void Player_BeginPositionRequest(void *, void *, s32, s32, s32, s32, s32, s32, s32);
void Player_ApplyPositionRequest(void *, void *, s32, s32, s32, s32, s32, s32);
static inline void Pair_StepSequence(struct PairHalf *owner)
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
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
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

static inline u32 TestPlayerFlag(u32 mask, struct CapturePlayer *player)
{
    return player->flags & mask;
}
static inline u32 BeginCaptureRequest(struct CapturePlayer *player,
                                      struct MotionTrackVector *position, u32 apply, u32 mode)
{
    if (TestPlayerFlag(0x40000, player))
        return 0;
    Player_BeginPositionRequest(player, position, 0, 0, apply, 0, 0, 0, mode);
    return 1;
}
static inline void ChangeCapturePhase(struct PairHalf *half, u32 phase, u8 *entered)
{
    half->phase = phase;
    half->timer = 0;
    *entered = 1;
}
static inline void SetCaptureAnimation(struct PairHalf *half, void *resource, u32 index, u32 zero)
{
    Anim_SetSequence(&half->sequence, resource, index, (u8)half->mode, zero);
}
static inline void initVector(struct MotionTrackVector *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
static inline void copyPosition(struct MotionTrackVector *dst, struct MotionTrackVector *src)
{
    dst->x = src->x;
    dst->y = src->y - 128;
    dst->z = src->z;
}
static inline u8 trackFinished(struct PairHalf *half)
{
    return half->track.active == 2;
}
static inline void setPhase(struct PairHalf *half, u32 phase)
{
    half->phase = phase;
    half->timer = 0;
    half->phaseEntered = 1;
}
void PairedSceneActor_UpdatePlayerCapture(struct PairOwner *owner, struct PairHalf *half)
{
    struct MotionTrackVector target;
    struct MotionTrackVector position;
    {
        u32 mask = 12;
        half->controlFlags |= mask;
    }
    if (half->entered) {
        half->entered = 0;
        {
            void *resource = owner->resource;
            Anim_SetSequence(&half->sequence, resource, 12, (u8)half->mode, 0);
        }
        Pair_StepSequence(half);
        setPhase(half, 0);
    }
    switch (half->phase) {
        case 0: {
            u32 entered = half->phaseEntered;
            u8 *phaseEntered = &half->phaseEntered;
            if (entered) {
                u32 zero = 0;
                *phaseEntered = zero;
                initVector(&target, 384, zero, 384);
                MotionTrack_Initialize(&half->track, &half->offset, &target, zero, 4);
            }
            copyPosition(&position, &half->position);
            if (half->mode == 0) {
                if (!BeginCaptureRequest(*(struct CapturePlayer **)gPlayers, &position, 1, 1))
                    goto abortCapture;
            } else if (!BeginCaptureRequest(*(struct CapturePlayer **)gPlayers, &position, 0, 2))
                goto abortCapture;
            goto advanceTrack;
        abortCapture:
            ChangeCapturePhase(half, 3, phaseEntered);

            break;
        advanceTrack:
            MotionTrack_Update(&half->track);

            if (trackFinished(half)) {
                ChangeCapturePhase(half, 1, phaseEntered);
            }
            break;
        }
        case 1:
            if ((++half->timer) > 120) {
                {
                    void *resource = owner->resource;
                    SetCaptureAnimation(half, resource, 9, 0);
                }
                Pair_StepSequence(half);
                setPhase(half, 2);
            }
            break;

        case 2:
            Player_ApplyPositionRequest((*(struct CapturePlayer **)gPlayers), &half->position, 80,
                                        32, 128, 0, 0, 0);
            {
                void *resource = owner->resource;
                SetCaptureAnimation(half, resource, 9, 0);
            }
            Pair_StepSequence(half);
            setPhase(half, 3);
            break;

        case 3:
            half->finished = 1;
            break;
    }
}
