#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u32 flags;
    u8 visible, unknown5, palette, unknown7;
    s8 scaleX, scaleY;
    u8 padA[6];
    u16 frame;
    u8 pad12[10];
    struct Vec position;
    u8 pad24[0x24];
};
struct Anim {
    const volatile u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
static inline u32 Step(struct Sprite *sprite, struct Anim *state)
{
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
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
        /* Keep the live mask distinct from its source without binding r7. */

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
    return ended;
}

struct TrailSprite {
    struct Sprite sprite;
    struct Anim anim;
    u16 lifetime, unknown5A;
};
struct Effect {
    u8 unknown0[6];
    u8 palette;
    u8 unknown7[0x15];
    struct Vec position;
    u8 unknown24[0x18];
    struct TrailSprite trail[2];
    u8 unknownF4[0xB6];
    u8 next, interval;
    void *resource;
    u8 unknown1B0[8];
    u16 frame;
};
struct Packed {
    u32 a, b;
};
s32 Mod(s32, s32);
void Anim_SetSequence(struct Anim *, void *, u32, u32, u32);
// confidence=low: rotates two short-lived copies of the parent sprite as a trail.
void TwinSpriteEffect_EmitTrail(struct Effect *p)
{
    if (!Mod(p->frame, p->interval)) {
        struct Sprite *sprite;
        struct Anim *anim;
        u32 k;
        {
            u32 idx = p->next;
            k = 0x5c;
            Anim_SetSequence((void *)(idx * k + (u32)p + 0x84), p->resource, 1, 0, 0);
        }
        {
            register u32 sp asm("r5");
            u32 index = p->next;
            sp = index;
            sp = sp * k;
            sp = sp + (u32)p;
            sprite = (void *)(sp + 0x3c);
            k = k * index;
            anim = (void *)(k + (u32)p + 0x84);
        }
        Step(sprite, anim);
        {
            struct Packed *dest;
            u32 off;
            u8 *b;
            register u32 d1 asm("r1");
            register u32 d0 asm("r0");
            u8 *np;
            asm("" : "=r"(d1), "=r"(d0));
            np = &p->next;
            asm("" ::"r"(d0));
            off = *np * 0x5c;
            asm("" ::"r"(d1));
            b = (u8 *)p + 0x58;
            dest = (void *)(b + off);
            *dest = *(struct Packed *)&p->position;
        }
        p->trail[p->next].sprite.flags &= ~1;
        p->trail[p->next].sprite.flags |= 2;
        p->trail[p->next].sprite.palette = p->palette;
        p->trail[p->next].lifetime = 4;
        if (++p->next > 1)
            p->next = 0;
    }
}
