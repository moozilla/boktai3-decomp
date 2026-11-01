// MATCHING_CONCESSION: register-pinning
#include "sprite_effect_controller.h"
struct SequenceDrawable {
    u32 flags;
    u8 active;
    u8 unknown5[11];
    u16 frame;
};
struct TileDrawable {
    u32 flags;
    u8 active;
    u8 unknown5[3];
    u16 tile;
    u8 width, height;
    u8 unknownC[0x1c];
    const u16 *graphics;
};
u32 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
static inline void StepSprite(struct SpriteEffectController *input)
{
    struct SpriteEffectController *slot = input;
    struct SequenceDrawable *sprite;
    register struct SpriteEffectController *base asm("r12");
    register struct DisplaySequenceState *state asm("r12");
    const volatile u16 *command;
    u32 bit;
    u32 duration;
    u32 flags;
    base = slot;
    asm("" : "+r"(base));
    {
        u8 *advance = (void *)base;
        advance += 4;
        asm("" : "+r"(advance));
        state = (void *)advance;
    }
    {
        void **cursor = (void *)((u8 *)state - 4);
        sprite = *cursor++;
        {
            u32 offset = ((struct DisplaySequenceState *)cursor)->index << 1;
            const u16 *table = slot->sequence.commands;
            command = (void *)((u32)table + offset);
        }
    }
    sprite->frame = *command >> 6;
    {
        u32 input;
        register struct DisplaySequenceState *low asm("r1") = state;
        register u32 first asm("r1");
        u32 maskBit = 1;
        input = low->flags;
        bit = maskBit;
        asm("" : "+r"(bit));

        first = bit;
        first &= input;
        {
            u32 bits = *command;
            u32 flip = 0x30;
            flip &= bits;
            flip >>= 4;
            flip &= bit;

            if (first != flip)
                sprite->flags |= 4;
            else
                sprite->flags &= ~4;
        }
    }
    {
        u32 input;
        register struct DisplaySequenceState *low asm("r2") = state;
        u32 bit;
        register u32 first asm("r1");
        input = low->flags;
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
        register struct DisplaySequenceState *low asm("r3") = state;
        u32 elapsed;
        register u32 temp asm("r5");
        u32 mask;
        register u32 saved asm("r3");
        elapsed = low->elapsed + 1;
        low->elapsed = elapsed;
        temp = 65535;
        asm("" : "+r"(temp));
        mask = temp;
        asm("" : "+&r"(mask) : "r"(temp));
        duration = low->duration;
        elapsed = (u16)elapsed;
        saved = duration;
        if (elapsed >= duration) {
            {
                u32 zero = 0;
                register struct DisplaySequenceState *write asm("r1");
                asm("" : "+r"(zero));
                write = state;
                write->elapsed = zero;
                flags = write->flags;
            }
            {
                u32 reverse = flags & 4;
                if (reverse) {
                    struct DisplaySequenceState *read = state;
                    u32 index;
                    index = read->index;
                    if (index == 0) {
                        u32 hold = flags & 8;
                        asm("" : "+r"(hold));
                        if (hold)
                            read->elapsed = saved;
                        else {
                            register struct DisplaySequenceState *write asm("r3") = state;
                            u32 last;
                            last = write->frameCount - 1;
                            write->index = last;
                        }
                    } else {
                        register struct DisplaySequenceState *write asm("r5");
                        index--;
                        write = state;
                        write->index = index;
                    }
                } else {
                    register struct DisplaySequenceState *write asm("r3") = state;
                    u32 index;
                    u32 count;
                    index = write->index + 1;
                    write->index = index;
                    count = write->frameCount;
                    if ((index & mask) >= count) {
                        u32 hold = flags & 8;
                        asm("" : "+r"(hold));
                        hold = (u8)hold;
                        if (hold) {
                            register struct DisplaySequenceState *write asm("r5") = state;
                            u16 last;
                            write->elapsed = duration;
                            last = count - 1;
                            write->index = last;
                        } else {
                            register struct DisplaySequenceState *write asm("r1") = state;
                            write->index = hold;
                        }
                    }
                }
            }
            {
                struct DisplaySequenceState *write = state;
                register u32 offset asm("r0");
                register const u16 *table asm("r1");
                asm("" : "+r"(write));
                offset = write->index << 1;
                table = write->commands;
                command = (void *)((u32)table + offset);
                write->frameWeight = *command & 15;
                {
                    register s32 weight asm("r1") = write->frameWeight;
                    s32 value = write->speed;
                    value = (value * weight) >> 6;
                    write->duration = value;
                    if (!(u8)value)
                        write->duration = 1;
                }
            }
        }
    }
    return;
}
static inline void StepEffect(struct SpriteEffectController *input)
{
    struct SpriteEffectController *slot = input;
    struct TileDrawable *sprite;
    register struct SpriteEffectController *base asm("r12");
    register struct DisplaySequenceState *state asm("r12");
    const volatile u16 *command;
    u32 bit;
    u32 duration;
    u32 flags;
    base = slot;
    asm("" : "+r"(base));
    {
        register u8 *advance asm("r3") = (void *)base;
        advance += 4;
        asm("" : "+r"(advance));
        state = (void *)advance;
    }
    {
        register void **cursor asm("r3") = (void *)((u8 *)state - 4);
        const u16 *graphics;
        register struct DisplaySequenceState *read asm("r5");
        u32 offset;
        const u16 *table;
        sprite = *cursor++;
        asm volatile("" : : "r"(cursor));
        graphics = sprite->graphics;
        read = state;
        offset = read->index << 1;
        table = slot->sequence.commands;
        command = (void *)((u32)table + offset);
        {
            u32 frame = *command >> 6;
            sprite->tile = (sprite->width >> 3) * (sprite->height >> 3) * frame + graphics[2];
        }
    }
    {
        u32 input;
        register struct DisplaySequenceState *low asm("r1") = state;
        register u32 first asm("r1");
        u32 maskBit = 1;
        input = low->flags;
        bit = maskBit;
        asm("" : "+r"(bit));

        first = bit;
        first &= input;
        {
            u32 bits = *command;
            u32 flip = 0x30;
            flip &= bits;
            flip >>= 4;
            flip &= bit;

            if (first != flip)
                sprite->flags |= 4;
            else
                sprite->flags &= ~4;
        }
    }
    {
        u32 input;
        register struct DisplaySequenceState *low asm("r2") = state;
        u32 bit;
        register u32 first asm("r1");
        input = low->flags;
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
        register struct DisplaySequenceState *low asm("r3") = state;
        u32 elapsed;
        register u32 temp asm("r5");
        u32 mask;
        register u32 saved asm("r3");
        elapsed = low->elapsed + 1;
        low->elapsed = elapsed;
        temp = 65535;
        asm("" : "+r"(temp));
        mask = temp;
        asm("" : "+&r"(mask) : "r"(temp));
        duration = low->duration;
        elapsed = (u16)elapsed;
        saved = duration;
        if (elapsed >= duration) {
            {
                u32 zero = 0;
                register struct DisplaySequenceState *write asm("r1");
                asm("" : "+r"(zero));
                write = state;
                write->elapsed = zero;
                flags = write->flags;
            }
            {
                u32 reverse = flags & 4;
                if (reverse) {
                    struct DisplaySequenceState *read = state;
                    u32 index;
                    index = read->index;
                    if (index == 0) {
                        u32 hold = flags & 8;
                        asm("" : "+r"(hold));
                        if (hold)
                            read->elapsed = saved;
                        else {
                            register struct DisplaySequenceState *write asm("r3") = state;
                            u32 last;
                            last = write->frameCount - 1;
                            write->index = last;
                        }
                    } else {
                        register struct DisplaySequenceState *write asm("r5");
                        index--;
                        write = state;
                        write->index = index;
                    }
                } else {
                    register struct DisplaySequenceState *write asm("r3") = state;
                    u32 index;
                    u32 count;
                    index = write->index + 1;
                    write->index = index;
                    count = write->frameCount;
                    if ((index & mask) >= count) {
                        u32 hold = flags & 8;
                        asm("" : "+r"(hold));
                        hold = (u8)hold;
                        if (hold) {
                            register struct DisplaySequenceState *write asm("r5") = state;
                            u16 last;
                            write->elapsed = duration;
                            last = count - 1;
                            write->index = last;
                        } else {
                            register struct DisplaySequenceState *write asm("r1") = state;
                            write->index = hold;
                        }
                    }
                }
            }
            {
                struct DisplaySequenceState *write = state;
                u32 offset;
                const u16 *table;
                offset = write->index << 1;
                table = write->commands;
                command = (void *)((u32)table + offset);
                {
                    s32 value = *command & 15;
                    u32 speed = write->speed;
                    value = (value * (s32)speed) >> 6;
                    write->duration = value;
                }
            }
        }
    }
    return;
}
void SpriteEffectController_FadeAndDestroy(struct SpriteEffectControllerPool *unused,
                                           struct SpriteEffectController *input)
{
    struct SpriteEffectController *slot = input;
    u16 *flags;
    {
        u8 *enter = &slot->enter;
        if (*enter) {
            u32 zero = 0;
            u16 *frame;
            *enter = zero;
            frame = &slot->frame;
            *frame = zero;
        }
    }
    {
        u16 *read = &slot->flags;
        u32 type;
        {
            u32 bits = *read;
            type = 8;
            type &= bits;
        }
        flags = read;
        if (type)
            StepSprite(slot);
        else
            StepEffect(slot);
    }
    {
        u32 bits = *flags;
        u32 selected = 8;
        selected &= bits;
        asm volatile("" : : "r"(selected));
    }
    {
        void *drawable = slot->drawable;
        u16 *age = &slot->frame;
        if ((u8)Sprite_BlinkUntilExpired(drawable, *age, 60, 40)) {
            struct SpriteEffectControllerPool *p = gSpriteEffectControllerPool;
            register s8 *index asm("r3") = &slot->index;
            p->used &= ~(1U << *index);
            slot->state = 0;
            {
                u32 type;
                register s8 *saved asm("r4");
                {
                    u32 bits = *flags;
                    type = 8;
                    type &= bits;
                }
                saved = index;
                if (type) {
                    struct SequenceDrawable *s = slot->drawable;
                    if (s->active) {
                        Sprite_Remove(s);
                        Heap_Free(slot->drawable);
                    }
                } else {
                    struct TileDrawable *s = slot->drawable;
                    if (s->active) {
                        Effect_Remove(s);
                        Heap_Free(slot->drawable);
                    }
                }
                *(u8 *)saved = 255;
                *flags = 0;
            }
        } else
            (*age)++;
    }
}
