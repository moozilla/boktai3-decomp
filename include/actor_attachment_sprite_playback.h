#ifndef GUARD_ACTOR_ATTACHMENT_SPRITE_PLAYBACK_H
#define GUARD_ACTOR_ATTACHMENT_SPRITE_PLAYBACK_H
#include "actor_attachment_state.h"
#include "display_sequence_inline.h"
/* DisplaySequence_StepSprite (display_sequence_sprite.h) for the slot's own sprite, using the
 * caller's state pointer directly: the descending and ascending updaters keep it in one register.
 * Returns 1 when the sequence reached its end (or start, when reversed) this step. */
static inline u32 ActorAttachment_StepSprite(struct ContactSwarmSprite *sprite,
                                             struct DisplaySequenceState *state)
{
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

#include "engine.h"

#endif
