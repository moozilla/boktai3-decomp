#ifndef GUARD_DISPLAY_SEQUENCE_INLINE_H
#define GUARD_DISPLAY_SEQUENCE_INLINE_H

#include "display_sequence.h"

static inline u16 DisplaySequence_GetIndex(struct DisplaySequenceState *state)
{
    return state->index;
}

static inline u32 DisplaySequence_ReadCommand(const u16 *command)
{
    return *command;
}

static inline u32 DisplaySequence_MaskCommand(u32 command, u32 mask)
{
    mask &= command;
    return mask;
}

static inline void DisplaySequence_ApplyFrame(struct SceneActorControlMotionDrawable *owner,
                                              struct DisplaySequenceState *state,
                                              const u16 *command)
{
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
}

static inline void DisplaySequence_SelectNext(struct DisplaySequenceState *state)
{
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
}

static inline void DisplaySequence_UpdateWeight(struct DisplaySequenceState *state,
                                                const u16 *command)
{
    state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
    state->duration = (state->frameWeight * (s32)state->speed) >> 6;
}

/* Apply the current frame and flips, then advance the stored playback state.
 * The duration calculation uses the newly selected command's low four bits. */
static inline void DisplaySequence_Step(void *obj)
{
    struct SceneActorControlMotionDrawable *owner = obj;
    struct DisplaySequenceState *state;
    const u16 *command;
    state = &owner->sequence;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = owner->sequence.commands;
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

static inline void DisplaySequence_StepState(struct SceneActorControlMotionDrawable *drawable,
                                             struct DisplaySequenceState *sequence)
{
    struct SceneActorControlMotionDrawable *owner = drawable;
    struct DisplaySequenceState *state;
    const u16 *command;
    state = sequence;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = owner->sequence.commands;
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
#endif
