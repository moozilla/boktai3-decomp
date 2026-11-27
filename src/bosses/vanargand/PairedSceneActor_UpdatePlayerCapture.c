#include "bosses/vanargand/paired_scene_actor.h"
#include "global.h"
#include "engine/video/display_animation.h"
#include "engine/video/display_sequence_inline.h"
#include "player/player.h"
#include "scene/actor/motion_track.h"
#include "bosses/vanargand/vanargand_parts.h"
// confidence=low: moves a paired half toward the player, then holds and releases the position request.
void MotionTrack_Update(void *);
void Player_BeginPositionRequest(void *, void *, s32, s32, s32, s32, s32, s32, s32);
void Player_ApplyPositionRequest(void *, void *, s32, s32, s32, s32, s32, s32);
static inline void Pair_StepSequence(struct PairedSceneHalf *owner)
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

static inline u32 TestPlayerFlag(u32 mask, struct Player *player)
{
    return player->statusFlags & mask;
}
static inline u32 BeginCaptureRequest(struct Player *player, struct MotionTrackVector *position,
                                      u32 apply, u32 mode)
{
    if (TestPlayerFlag(0x40000, player))
        return 0;
    Player_BeginPositionRequest(player, position, 0, 0, apply, 0, 0, 0, mode);
    return 1;
}
static inline void ChangeCapturePhase(struct PairedSceneHalf *half, u32 phase, u8 *entered)
{
    half->actionCounters[0] = phase;
    half->actionCounters[1] = 0;
    *entered = 1;
}
static inline void SetCaptureAnimation(struct PairedSceneHalf *half, void *resource, u32 index,
                                       u32 zero)
{
    Anim_SetSequence(&half->graphics.sequence, resource, index, (u8)half->mode, zero);
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
void PairedSceneActor_UpdatePlayerCapture(struct VanargandAttackPair *owner,
                                          struct PairedSceneHalf *half)
{
    struct MotionTrackVector target;
    struct MotionTrackVector position;
    {
        u32 mask = 12;
        half->actionFlags |= mask;
    }
    if (half->entered) {
        half->entered = 0;
        {
            void *resource = (void *)owner->effectResource;
            Anim_SetSequence(&half->graphics.sequence, resource, 12, (u8)half->mode, 0);
        }
        Pair_StepSequence(half);
        PairedSceneHalf_BeginActionPhase(half, 0);
    }
    switch (half->actionCounters[0]) {
        case 0: {
            u32 entered = half->actionEntered;
            u8 *phaseEntered = &half->actionEntered;
            if (entered) {
                u32 zero = 0;
                *phaseEntered = zero;
                initVector(&target, 384, zero, 384);
                MotionTrack_Initialize(&half->motion, &half->offset, &target, zero, 4);
            }
            copyPosition(&position, (struct MotionTrackVector *)&half->sprite.position);
            if (half->mode == 0) {
                if (!BeginCaptureRequest(*(struct Player **)gPlayers, &position, 1, 1))
                    goto abortCapture;
            } else if (!BeginCaptureRequest(*(struct Player **)gPlayers, &position, 0, 2))
                goto abortCapture;
            goto advanceTrack;
        abortCapture:
            ChangeCapturePhase(half, 3, phaseEntered);

            break;
        advanceTrack:
            MotionTrack_Update(&half->motion);

            if (Vanargand_MotionEnded(half)) {
                ChangeCapturePhase(half, 1, phaseEntered);
            }
            break;
        }
        case 1:
            if ((++half->actionCounters[1]) > 120) {
                {
                    void *resource = (void *)owner->effectResource;
                    SetCaptureAnimation(half, resource, 9, 0);
                }
                Pair_StepSequence(half);
                PairedSceneHalf_BeginActionPhase(half, 2);
            }
            break;

        case 2:
            Player_ApplyPositionRequest(gPlayers[0], &half->sprite.position, 80, 32, 128, 0, 0, 0);
            {
                void *resource = (void *)owner->effectResource;
                SetCaptureAnimation(half, resource, 9, 0);
            }
            Pair_StepSequence(half);
            PairedSceneHalf_BeginActionPhase(half, 3);
            break;

        case 3:
            half->finished = 1;
            break;
    }
}
