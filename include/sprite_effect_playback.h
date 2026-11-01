#ifndef GUARD_SPRITE_EFFECT_PLAYBACK_H
#define GUARD_SPRITE_EFFECT_PLAYBACK_H
#include "sprite_effect_controller.h"
/* Packed sequence playback for the two drawable variants. Sequence sprites
 * cache frame weight and clamp zero delay to one; tile sprites do neither. */
static inline u32 SpriteEffectController_StepDrawable(struct SpriteEffectController *controller,
                                                      u32 sequenceMode)
{
    void *const *prefix = (void *const *)controller;
    void *raw = *prefix++;
    struct DisplaySequenceState *state = (struct DisplaySequenceState *)prefix;
    struct SpriteEffectDrawablePrefix *drawable = raw;
    const struct SpriteEffectTileResource *graphics;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended;
    if (!sequenceMode)
        graphics = ((struct SpriteEffectTileDrawable *)raw)->graphics;
    command = state->commands + state->index;
    if (sequenceMode)
        ((struct SpriteEffectSequenceDrawable *)raw)->frame = *command >> 6;
    else {
        struct SpriteEffectTileDrawable *tile = raw;
        u32 frame = *command >> 6;
        u32 size = (tile->width >> 3) * (tile->height >> 3);
        tile->tile = size * frame + graphics->tileBase;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            drawable->flags |= 4;
        else
            drawable->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            drawable->flags |= 8;
        else
            drawable->flags &= ~8;
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
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
                ended = 1;
            } else {
                ended = 0;
            }
        }
        command = state->commands + state->index;
        if (sequenceMode) {
            state->frameWeight = *command & 15;
            state->duration = (state->frameWeight * (s32)state->speed) >> 6;
            if (state->duration == 0)
                state->duration = 1;
        } else
            state->duration = ((*command & 15) * (s32)state->speed) >> 6;
    } else {
        ended = 0;
    }
    return ended;
}

#endif
