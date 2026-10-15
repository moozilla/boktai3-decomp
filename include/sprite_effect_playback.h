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
    const volatile u16 *command;
    u32 ended, duration, flags;
    if (!sequenceMode)
        graphics = ((struct SpriteEffectTileDrawable *)raw)->graphics;
    command = state->commands + state->index;
    if (sequenceMode)
        ((struct SpriteEffectSequenceDrawable *)raw)->frame = *command >> 6;
    else {
        struct SpriteEffectTileDrawable *tile = raw;
        u32 frame = *command >> 6;
        u32 width = tile->width >> 3;
        register u32 height asm("r0") = tile->height >> 3;
        height *= width;
        height *= frame;
        height += graphics->tileBase;
        tile->tile = height;
    }
    {
        u32 input = state->flags;
        u32 bit = 1;
        register u32 first asm("r1");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            drawable->flags |= 4;
        else
            drawable->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit = 2;
        register u32 first asm("r1");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            drawable->flags |= 8;
        else
            drawable->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        if (sequenceMode) {
            register u32 temp asm("r1") = 0xFFFF;
            asm("" : "+r"(temp));
            mask = temp;
            asm("" : "+&r"(mask) : "r"(temp));
        } else {
            register u32 temp asm("r2") = 0xFFFF;
            asm("" : "+r"(temp));
            mask = temp;
            asm("" : "+&r"(mask) : "r"(temp));
        }
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (!index) {
                    u32 hold = flags & 8;
                    asm("" : "+r"(hold));
                    if (hold)
                        state->elapsed = savedDuration;
                    else
                        state->index = state->frameCount - 1;
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    u32 hold = flags & 8;
                    asm("" : "+r"(hold));
                    if ((u8)hold) {
                        state->elapsed = duration;
                        state->index = frameCount - 1;
                    } else
                        state->index = 0;
                    ended = 1;
                } else
                    ended = 0;
            }
            command = state->commands + state->index;
            if (sequenceMode) {
                s32 delay;
                state->frameWeight = *command & 15;
                {
                    s32 weight = state->frameWeight;
                    s32 speed = state->speed;
                    delay = (speed * weight) >> 6;
                }
                state->duration = delay;
                if (!(u8)delay)
                    state->duration = 1;
            } else
                state->duration = ((*command & 15) * (s32)state->speed) >> 6;
        } else
            ended = 0;
    }
    return ended;
}

#endif
