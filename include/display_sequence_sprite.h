#ifndef GUARD_DISPLAY_SEQUENCE_SPRITE_H
#define GUARD_DISPLAY_SEQUENCE_SPRITE_H

#include "display_animation.h"
#include "display_sequence_inline.h"

/* Apply the current frame and flips to a separate sprite, then advance the playback state.
 * Returns 1 when the sequence reached its end (or start, when reversed) this step.
 * The local copy of the state parameter is a separate pseudo, which keeps the caller's own
 * state pointer at its ROM priority (PlayerAttackEffect_AllocateParticle). */
static inline u32 DisplaySequence_StepSprite(struct DisplaySequenceSprite *sprite,
                                             struct DisplaySequenceState *frames)
{
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
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
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

#endif
