// MATCHING_CONCESSION: register-pinning
#include "display_animation.h"
s32 DisplayAnimation_UpdatePlayback(struct DisplayAnimation *display)
{
    u32 kind = display->kind;
    if (kind == 0)
        return -1;
    if (kind == 1) {
        struct DisplaySequenceSprite *sprite;
        struct DisplaySequenceState *state;
        const volatile u16 *command; /* preserve observed halfword reads of packed commands */
        u32 ended;
        u32 duration;
        u32 flags;
        asm("" : "+r"(kind));
        if (!display->resource.sequence)
            goto done;
        sprite = (struct DisplaySequenceSprite *)((u8 *)display + 0x28);
        state = (struct DisplaySequenceState *)((u8 *)display + 0x54);
        {
            u32 offset = state->index << 1;
            const volatile u16 *table;
            table = *(const u16 **)((u8 *)display + 0x54);
            command = (const volatile u16 *)((u32)table + offset);
        }
        sprite->frame = *command >> 6;
        {
            u32 input = state->flags;
            u32 first;
            first = kind;
            asm("" : "+r"(first));
            first &= input;
            if (first != (((*command & 0x30) >> 4) & kind))
                sprite->flags |= 4;
            else
                sprite->flags &= ~4;
        }
        {
            u32 input = state->flags;
            u32 bit;
            register u32 first asm("r1");
            bit = 2;
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
            register u32 mask asm("r5");
            register u32 temp asm("r1");
            register u32 savedDuration asm("r3");
            state->elapsed = elapsed;
            temp = 0xFFFF;
            asm("" : "+r"(temp));
            mask = temp;
            duration = state->duration;
            elapsed = (u16)elapsed;
            savedDuration = duration;
            if (elapsed >= duration) {
                state->elapsed = 0;
                flags = state->flags;
                if (flags & 4) {
                    u32 index = state->index;
                    if (index == 0) {
                        u32 hold = 8;
                        hold &= flags;
                        if (hold) {
                            state->elapsed = savedDuration;
                        } else {
                            state->index = state->frameCount - 1;
                        }
                        ended = 1;
                    } else {
                        state->index = index - 1;
                        ended = 0;
                    }
                } else {
                    u32 index = state->index + 1;
                    u32 frameCount;
                    u32 endFlag;
                    state->index = index;
                    frameCount = state->frameCount;
                    if ((index & mask) >= frameCount) {
                        endFlag = flags & 8;
                        asm("" : "+r"(endFlag));
                        endFlag = (u8)endFlag;
                        if (endFlag) {
                            state->elapsed = duration;
                            {
                                u32 lastIndex = frameCount - 1;
                                state->index = lastIndex;
                            }
                        } else {
                            state->index = 0;
                        }
                        ended = 1;
                    } else {
                        ended = 0;
                    }
                }
                {
                    u32 offset = state->index << 1;
                    const volatile u16 *table;
                    table = state->commands;
                    command = (const volatile u16 *)((u32)table + offset);
                }
                state->frameWeight = *command & 15;
                {
                    register s32 weight asm("r1") = state->frameWeight;
                    s32 speed;
                    speed = state->speed;
                    speed = (speed * weight) >> 6;
                    state->duration = speed;
                    if (!(u8)speed)
                        state->duration = 1;
                }
            } else {
                ended = 0;
            }
        }
        display->finished = ended;
    } else {
        void *resource = &display->resource;
        display->finished = Anim_Step((u8 *)display + 0x2C, resource);
    }
done:
    return display->finished;
}
