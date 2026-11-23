#include "global.h"
#include "tentacle_segment_animation.h"
#include "display_animation.h"
#include "display_sequence_inline.h"

void *SceneActorProfiles_GetEntryAddress(u32, u32, s32);
void TentacleBody_GetDirectionFrame(u16, s32 *, s32 *);
void Anim_RestartSequence(void *, void *, u32, u8, u8);
void Anim_SetDurationScale(void *, u32);
static inline void Step(struct SceneActorControlMotionDrawable *drawable,
                        struct DisplaySequenceState *sequence)
{
    struct SceneActorControlMotionDrawable *owner = drawable;
    struct DisplaySequenceState *state;
    const u16 *command;
    state = sequence;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    DisplaySequence_ApplyFrame(owner, state, command);
    if (++state->elapsed >= state->duration) {
        DisplaySequence_SelectNext(state);
        command = state->commands + state->index;
        DisplaySequence_UpdateWeight(state, command);
        if (state->duration == 0)
            state->duration = 1;
    }
}

void TentacleBody_RestartSegmentAnimations(struct TentacleBodyAnimationView *body, s32 kind,
                                           s32 row, s32 flags)
{
    struct TentacleBodyAnimationView *actor;
    u8 *profile;
    s32 i;
    actor = body;
    profile = SceneActorProfiles_GetEntryAddress(body->profileId, body->profileMode, 0);
    for (i = 0; i < body->segmentCount; i++) {
        if (!TentacleSegment_HasStatus(&body->segments[i].animation, 1)) {
            if (TentacleSegment_HasStatus(&body->segments[i].animation, 4))
                Anim_RestartSequence(&body->segments[i].animation,
                                     body->segments[i].animation.script, 1, row, flags);
            else {
                Anim_RestartSequence(&body->segments[i].animation,
                                     body->segments[i].animation.script, profile[kind * 4], row,
                                     flags);
                Anim_SetDurationScale(&body->segments[i].animation, actor->speed);
            }
            Step((void *)&body->segments[i], &body->segments[i].animation.sequence);
        }
    }
}
