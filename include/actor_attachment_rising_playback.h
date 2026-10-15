#ifndef GUARD_ACTOR_ATTACHMENT_RISING_PLAYBACK_H
#define GUARD_ACTOR_ATTACHMENT_RISING_PLAYBACK_H
#include "actor_attachment_state.h"
/* Rising-part packed tile/flip/timing decoder; defined operand hints are
 * minimized with its complete caller. The reverse paths share decrement. */
static inline void
ActorAttachment_StepRisingPart(struct ActorAttachmentPartSprite *sprite,
                               struct DisplaySequenceState *state,
                               const struct ActorAttachmentPartResource *resource)
{

    const volatile u16 *command;
    u32 duration, flags;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;

        table = state->commands;

        command = (const volatile u16 *)((u32)table + offset);
    }
    {
        u32 frame = *command >> 6;
        register u32 width asm("r1") = sprite->width >> 3;
        u32 height = sprite->height >> 3;
        height *= width;
        height *= frame;
        height += resource->tileBase;
        sprite->tile = height;
    }
    {
        u32 input = state->flags;
        u32 bit = 1;
        register u32 first asm("r1");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
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
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 cache asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        asm("" : "+&r"(mask) : "r"(temp));
        duration = state->duration;
        elapsed = (u16)elapsed;
        cache = duration;
        asm("" : : "r"(cache));
        if ((u16)elapsed >= duration) {
            {
                u32 zero = 0;
                state->elapsed = zero;
            }
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (!index) {
                    u32 hold = 8;
                    hold &= flags;

                    if (hold)
                        state->elapsed = cache;
                    else {
                        index = state->frameCount;
                        goto decrement;
                    }
                } else {
                decrement:
                    state->index = index - 1;
                }
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
