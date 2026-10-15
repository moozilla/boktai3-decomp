#ifndef GUARD_ACTOR_ATTACHMENT_ORBIT_PLAYBACK_H
#define GUARD_ACTOR_ATTACHMENT_ORBIT_PLAYBACK_H
#include "actor_attachment_state.h"
/* Ordinary-C packed tile/flip/timing playback shared by both orbit paths.
 * Defined register/operand concessions were minimized with the full updater. */
static inline void
ActorAttachment_StepOrbitPart(struct ActorAttachmentPartSprite *part,
                              struct DisplaySequenceState *sequence,
                              const struct ActorAttachmentPartResource *inputResource)
{
    register struct ActorAttachmentPartSprite *sprite asm("r6") = part;
    register const struct ActorAttachmentPartResource *resource asm("r12") = inputResource;

    struct ActorAttachmentPartSprite *copy = sprite;
    struct DisplaySequenceState *state = sequence;
    const volatile u16 *command;
    u32 duration;
    u32 flags;

    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;

        command = (const volatile u16 *)((u32)table + offset);
    }
    {
        u32 frame = *command >> 6;
        u32 width = sprite->width >> 3;
        u32 height = sprite->height >> 3;
        height *= width;
        height *= frame;
        asm("" : "+r"(height) : : "r1");
        height += resource->tileBase;
        sprite->tile = height;
    }
    {
        u32 input = state->flags;
        u32 bit = 1;
        u32 first;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & 1))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit = 2;
        register u32 first asm("r1");
        first = bit;
        first &= input;

        first = (u8)first;
        {
            u32 orientation = *command;
            u32 flip;

            flip = ((orientation & 0x30) >> 4) & 2;

            if (first != flip)
                copy->flags |= 8;
            else
                copy->flags &= ~8;
        }
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r2");
        register u32 cache asm("r2");
        {
            u32 zero = 0;

            state->elapsed = elapsed;
        }
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;

        duration = state->duration;
        elapsed = (u16)elapsed;
        cache = duration;
        asm("" : : "r"(cache));
        if ((u16)elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (!index) {
                    u32 hold = 8;
                    hold &= flags;

                    if (hold)
                        state->elapsed = cache;
                    else
                        state->index = state->frameCount - 1;
                } else
                    state->index = index - 1;
            } else {
                u32 index = state->index + 1;
                state->index = index;
                cache = state->frameCount;

                if ((index & mask) >= cache) {
                    u32 endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        state->index = cache - 1;
                    } else
                        state->index = 0;
                }
            }
            asm("" : : "r"(cache));
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;

                command = (const volatile u16 *)((u32)table + offset);
            }
            state->duration = ((*command & 15) * (s32)state->speed) >> 6;
        }
    }
}
#endif
