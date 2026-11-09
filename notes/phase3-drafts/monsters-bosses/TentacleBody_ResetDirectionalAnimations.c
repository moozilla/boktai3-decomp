#include "global.h"
#include "tentacle_segment_animation.h"
#include "display_sequence_inline.h"

// confidence=low: refreshes segment animations from their direction.

void *SceneActorProfiles_GetEntryAddress(u32, u32, s32);
void TentacleBody_GetDirectionFrame(u16, s32 *, s32 *);
void Anim_SetSequence(void *, void *, u32, u8, u8);
void Anim_RestartSequence(void *, void *, u32, u8, u8);
void Anim_SetDurationScale(void *, u32);
static inline u8 TestMask(struct TentacleSegmentAnimation *p, u32 mask)
{
    u32 f = p->status;
    if (f & mask)
        return 1;
    return 0;
}
static inline void Step(struct TentacleSegmentAnimation *animation,
                        struct TentacleAnimationSegment *seg)
{
    struct DisplaySequenceState *state = &animation->sequence;
    const u16 *command;
    command = state->commands + state->index;
    seg->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            seg->flags |= 4;
        else
            seg->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            seg->flags |= 8;
        else
            seg->flags &= ~8;
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
        state->frameWeight = DisplaySequence_MaskCommand(*command, 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    }
}
void TentacleBody_ResetDirectionalAnimations(struct TentacleBodyAnimationView *input, s32 inputKind)
{
    struct TentacleBodyAnimationView *body = input;
    struct {
        s32 row, flags, kind;
        u8 *profile, *actor;
        s32 i;
    } frame;
    s32 offset;
    s32 four;
    struct TentacleSegmentAnimation *a;
    struct TentacleAnimationSegment *s;
    frame.kind = inputKind;
    frame.actor = (void *)body;
    frame.profile = SceneActorProfiles_GetEntryAddress(body->profileId, body->profileMode, 0);
    frame.i = 0;
    if (frame.i >= body->segmentCount)
        return;
    offset = 0;
    a = &body->segments[0].animation;
    s = body->segments;
    do {
        if (!TestMask(a, 1)) {
            if (TentacleSegment_HasStatus(a, 4)) {
                TentacleBody_GetDirectionFrame((a->direction + 128) & 255, &frame.row,
                                               &frame.flags);
                Anim_SetSequence(a, TentacleSegment_ScriptAt(body, offset), 1, frame.row,
                                 frame.flags);
            } else {
                TentacleBody_GetDirectionFrame(a->direction, &frame.row, &frame.flags);
                four = 4;
                Anim_SetSequence(a, TentacleSegment_ScriptAt(body, offset),
                                 frame.profile[frame.kind * four], frame.row, frame.flags);
                Anim_SetDurationScale(a, ((struct TentacleBodyAnimationView *)frame.actor)->speed);
            }
            Step(a, s);
        }
        offset += sizeof(struct TentacleAnimationSegment);
        a = (void *)((u8 *)a + sizeof(struct TentacleAnimationSegment));
        s = (void *)((u8 *)s + sizeof(struct TentacleAnimationSegment));
    } while (++frame.i < body->segmentCount);
}
