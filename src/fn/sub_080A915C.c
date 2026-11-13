#include "global.h"
#include "tentacle_segment_animation.h"
#include "display_animation.h"
#include "display_sequence_inline.h"
// MATCHING_CONCESSION: register-pinning

// confidence=low: refreshes segment animations from their direction.

void *SceneActorProfiles_GetEntryAddress(u32, u32, s32);
void TentacleBody_GetDirectionFrame(u16, s32 *, s32 *);
void Anim_RestartSequence(void *, void *, u32, u8, u8);
void Anim_SetDurationScale(void *, u32);
static inline u8 TestMask(struct TentacleSegmentAnimation *p, u32 mask)
{
    u32 f = p->status;
    if (f & mask)
        return 1;
    return 0;
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
    register struct TentacleAnimationSegment *s asm("r8");
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
                Anim_SetSequence(a, TentacleSegment_ScriptAt(body, offset), 1, (u8)frame.row,
                                 (u8)frame.flags);
            } else {
                TentacleBody_GetDirectionFrame(a->direction, &frame.row, &frame.flags);
                four = 4;
                Anim_SetSequence(a, TentacleSegment_ScriptAt(body, offset),
                                 *(u8 *)(({
                                             register s32 k asm("r2") = frame.kind;
                                             register s32 x asm("r0") = k * four;
                                             x;
                                         }) +
                                         (u32)frame.profile),
                                 (u8)frame.row, (u8)frame.flags);
                {
                    register u8 *act asm("r1") = frame.actor;
                    register u16 *ad asm("r0");
                    ad = &((struct TentacleBodyAnimationView *)act)->speed;
                    Anim_SetDurationScale(a, *ad);
                }
            }
            {
                register struct DisplaySequenceSprite *other asm("r6") = (void *)s;
                struct DisplaySequenceState *state = &a->sequence;
                const u16 *command;
                u32 ended;
                command = state->commands + state->index;
                other->frame = *command >> 6;
                {
                    union DisplaySequenceControl control;
                    u32 bit, first;
                    control.raw = state->flags;
                    bit = 1;
                    first = (u8)(control.raw & bit);
                    if (first != (((*command & 0x30) >> 4) & bit))
                        other->flags |= 4;
                    else {
                        u32 *flags = &s->flags;
                        register u32 value asm("r0") = *flags;
                        value &= ~4;
                        *flags = value;
                    }
                }
                {
                    union DisplaySequenceControl control;
                    u32 bit, first;
                    control.raw = state->flags;
                    bit = 2;
                    first = (u8)(control.raw & bit);
                    if (first != (((*command & 0x30) >> 4) & bit))
                        other->flags |= 8;
                    else
                        other->flags &= ~8;
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
                    } else if (++state->index >= state->frameCount) {
                        if ((u8)(state->flags & 8)) {
                            state->elapsed = state->duration;
                            state->index = state->frameCount - 1;
                            ended = 1;
                        } else {
                            state->index = 0;
                            ended = 1;
                        }
                    } else
                        ended = 0;
                    command = state->commands + state->index;
                    DisplaySequence_UpdateWeight(state, command);
                    if (!state->duration) {
                        u16 duration[] = {1};
                        state->duration = duration[0];
                    }
                } else
                    ended = 0;
            }
        }
        {
            register u32 step asm("r1") = sizeof(struct TentacleAnimationSegment);
            offset += step;
            a = (void *)((u8 *)a + step);
            s = (void *)((u8 *)s + step);
        }
    } while (({
                 register s32 t asm("r2") = frame.i + 1;
                 frame.i = t;
                 t;
             }) < body->segmentCount);
}
