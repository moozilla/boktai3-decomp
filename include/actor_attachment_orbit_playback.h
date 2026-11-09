#ifndef GUARD_ACTOR_ATTACHMENT_ORBIT_PLAYBACK_H
#define GUARD_ACTOR_ATTACHMENT_ORBIT_PLAYBACK_H
#include "actor_attachment_state.h"
/* Packed tile/flip/timing playback shared by both orbit paths. The sprite copy keeps the
 * second flip on its own register (the ROM's r5); the delay is not clamped. */
static inline void ActorAttachment_StepOrbitPart(struct ActorAttachmentPartSprite *sprite,
                                                 struct DisplaySequenceState *frames,
                                                 const struct ActorAttachmentPartResource *resource)
{
    struct ActorAttachmentPartSprite *copy = sprite;
    struct DisplaySequenceState *state = frames;
    const u16 *command;
    command = state->commands + state->index;
    sprite->tile = (*command >> 6) * ((sprite->width >> 3) * (sprite->height >> 3)) + resource->tileBase;
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
            copy->flags |= 8;
        else
            copy->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
            } else
                state->index--;
        } else if (++state->index >= state->frameCount) {
            if ((u8)(state->flags & 8)) {
                state->elapsed = state->duration;
                state->index = state->frameCount - 1;
            } else
                state->index = 0;
        }
        command = state->commands + state->index;
        state->duration = ((*command & 15) * state->speed) >> 6;
    }
}
#endif
