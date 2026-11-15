#include "global.h"
#include "tentacle_segment_animation.h"
#include "display_animation.h"
#include "display_sequence_inline.h"

// confidence=low: refreshes segment animations from their direction, keeping frame and timer.

void *SceneActorProfiles_GetEntryAddress(u32, u32, s32);
void TentacleBody_GetDirectionFrame(u16, s32 *, s32 *);
void Anim_RestartSequence(void *, void *, u32, u8, u8);
void Anim_SetDurationScale(void *, u32);
static inline u8 TestMask(struct TentacleSegmentAnimation *p)
{
    u32 f = p->status;
    if (f & 1)
        return 1;
    return 0;
}
static inline u32 Step(struct DisplaySequenceSprite *sprite, struct DisplaySequenceState *frames)
{
    struct DisplaySequenceSprite *other = sprite;
    struct DisplaySequenceState *state = frames;
    const u16 *command;
    u32 ended;
    command = state->commands + state->index;
    sprite->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
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
        if (!state->duration)
            state->duration = 1;
    } else
        ended = 0;
    return ended;
}

void TentacleBody_RefreshDirectionalAnimations(struct TentacleBodyAnimationView *body, s32 kind)
{
    s32 row, flags;
    u8 *profile;
    struct TentacleBodyAnimationView *actor;
    s32 i;
    u16 frameSave, timerSave;
    actor = body;
    profile = SceneActorProfiles_GetEntryAddress(body->profileId, body->profileMode, 0);
    for (i = 0; i < body->segmentCount; i++) {
        if (!TestMask(&body->segments[i].animation)) {
            if (TentacleSegment_HasStatus(&body->segments[i].animation, 4)) {
                TentacleBody_GetDirectionFrame((body->segments[i].animation.direction + 128) & 255,
                                               &row, &flags);
                Anim_SetSequence(&body->segments[i].animation,
                                 body->segments[i].animation.script, 1, (u8)row, (u8)flags);
            } else {
                TentacleBody_GetDirectionFrame(body->segments[i].animation.direction, &row, &flags);
                frameSave = body->segments[i].animation.sequence.index;
                timerSave = body->segments[i].animation.sequence.elapsed;
                Anim_SetSequence(&body->segments[i].animation,
                                 body->segments[i].animation.script, profile[kind * 4], (u8)row,
                                 (u8)flags);
                Anim_SetDurationScale(&body->segments[i].animation, actor->speed);
                body->segments[i].animation.sequence.index = frameSave;
                body->segments[i].animation.sequence.elapsed = timerSave;
            }
            Step((struct DisplaySequenceSprite *)&body->segments[i],
                 &body->segments[i].animation.sequence);
        }
    }
}
