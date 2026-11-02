#ifndef GUARD_ACTOR_ATTACHMENT_RISING_PLAYBACK_H
#define GUARD_ACTOR_ATTACHMENT_RISING_PLAYBACK_H
#include "actor_attachment_state.h"
#include "display_sequence_inline.h"
/* Rising-part packed tile/flip/timing decoder: the tile is the frame's cell offset from the
 * resource's tile base; the delay is not clamped. */
static inline void
ActorAttachment_StepRisingPart(struct ActorAttachmentPartSprite *sprite,
                               struct DisplaySequenceState *state,
                               const struct ActorAttachmentPartResource *resource)
{
    const u16 *command = state->commands + state->index;
    {
        u32 frame = *command >> 6;
        u32 size = (sprite->width >> 3) * (sprite->height >> 3);
        sprite->tile = size * frame + resource->tileBase;
    }
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
        DisplaySequence_SelectNext(state);
        command = state->commands + state->index;
        state->duration = ((s32)DisplaySequence_MaskCommand(*command, 15) * state->speed) >> 6;
    }
}
#endif
