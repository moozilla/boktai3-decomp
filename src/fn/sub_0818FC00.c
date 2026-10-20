// MATCHING_CONCESSION: register-pinning
#define NIDHOGGR_MASK "r6"
#include "global.h"
#include "display_animation.h"
struct NidhoggrVector {
    s16 x, y, z, pad;
};
struct NidhoggrSprite {
    u32 flags;
    u8 pad4[12];
    u16 frame;
    u8 pad12[10];
    struct NidhoggrVector position;
    u8 pad24[8];
};
struct NidhoggrEffect {
    void *update, *destroy;
    u8 animation[0x1c];
    struct NidhoggrSprite sprite;
    struct DisplaySequenceState sequence;
    u8 pad60[0x114];
    void *anchor;
    struct NidhoggrVector velocity;
    u8 state, age;
    s16 ground;
    u8 *boss;
};
#ifndef NIDHOGGR_MASK
#define NIDHOGGR_MASK "r5"
#endif
static inline u32 Nidhoggr_StepAnimation(struct NidhoggrEffect *owner)
{
    struct NidhoggrSprite *sprite = &owner->sprite;
    struct DisplaySequenceState *state = &owner->sequence;
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
        register u32 mask asm(NIDHOGGR_MASK);
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Preserve the observed constant-to-mask copy. */

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

struct EffectCollider {
    u8 pad[6];
    u16 flags;
};
#define Nidhoggr_UpdateDetachedAttackEffect sub_0818FC00
void Nidhoggr_UpdateDetachedAttackEffect(struct NidhoggrEffect *p)
{
    u32 ended = Nidhoggr_StepAnimation(p);
    register u32 offset asm("r2") = 0x180;
    u8 *state;
    u32 old;
    asm("" : "+r"(offset));
    state = (u8 *)p + offset;
    old = *state;
    switch (old) {
        case 0:
            if (ended) {
                struct EffectCollider *collider;
                u32 mask;
                p->anchor = 0;
                p->velocity.x = 12;
                p->velocity.y = old;
                p->velocity.z = old;
                p->ground = p->sprite.position.x;
                (*state)++;
                collider = (void *)((u8 *)p + 0xcc);
                mask = ~4;
                collider->flags &= mask;
            }
            break;
        case 1:
            if (p->sprite.position.x - p->ground > 0x1000) {
                struct EffectCollider *collider;
                p->update = 0;
                if (p->destroy) {
                    Callback_InvokeActor(p, p->destroy);
                    p->destroy = 0;
                }
                collider = (void *)((u8 *)p + 0xcc);
                collider->flags |= 4;
            }
            break;
    }
}
