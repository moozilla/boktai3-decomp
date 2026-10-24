// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: owner allocation, packed-coordinate temporaries and pointer copies.
struct NidhoggrPoolSequence {
    const volatile u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct NidhoggrEffectMotion {
    u16 x, y, z, unknown6;
};
struct NidhoggrPoolSprite {
    u32 flags;
    u8 unknown4[12];
    u16 frame;
    u8 unknown12[10];
    struct NidhoggrEffectMotion position;
    u8 unknown24[8];
};
struct NidhoggrAnimatedSlot {
    void (*update)(struct NidhoggrAnimatedSlot *);
    void (*onComplete)(struct NidhoggrAnimatedSlot *);
    u8 animationData[0x1c];
    struct NidhoggrPoolSprite sprite;
    struct NidhoggrPoolSequence sequence;
    struct NidhoggrEffectMotion motion;
    u8 lifetime;
    u8 unknown69[3];
};
s32 Callback_InvokeActor(void *, void *);
static inline void StepSequence(struct NidhoggrAnimatedSlot *owner)
{
    struct NidhoggrPoolSprite *sprite = &owner->sprite;
    struct NidhoggrPoolSequence *state = &owner->sequence;
    const volatile u16 *command = state->commands + state->index;
    u32 duration, flags, bit;
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 one = 1;
        bit = one;
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
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        asm("" : "+r"(mask));
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
                    if (hold)
                        state->elapsed = savedDuration;
                    else
                        state->index = state->frameCount - 1;
                } else {
                    state->index = index - 1;
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
                        state->index = frameCount - 1;
                    } else
                        state->index = 0;
                }
            }
            command = state->commands + state->index;
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        }
    }
}

struct EffectDelta {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
inline static void StepSequence_StepMovingSpriteEffect(struct NidhoggrAnimatedSlot *owner)
{
    struct NidhoggrPoolSprite *sprite = &owner->sprite;
    struct NidhoggrPoolSequence *state = &owner->sequence;
    const volatile u16 *command = state->commands + state->index;
    u32 duration;
    u32 flags;
    u32 bit;
    do {
        sprite->frame = (*command) >> 6;
    } while (0);
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 one = 1;
        bit = one;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
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
        if (first != ((((*command) & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        asm("" : "+r"(mask));
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
                    if (hold)
                        state->elapsed = savedDuration;
                    else
                        state->index = state->frameCount - 1;
                } else {
                    state->index = index - 1;
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
                        state->index = frameCount - 1;
                    } else
                        state->index = 0;
                }
            }
            command = state->commands + state->index;
            state->frameWeight = (*command) & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!((u8)speed))
                    state->duration = 1;
            }
        }
    }
}

void Nidhoggr_StepMovingSpriteEffect(struct NidhoggrAnimatedSlot *ownerArg)
{
    struct NidhoggrAnimatedSlot *owner = ownerArg;
    union {
        struct EffectDelta bits;
        struct NidhoggrEffectMotion words;
        u32 packed[2];
    } delta;
    struct NidhoggrEffectMotion *motion;
    struct NidhoggrEffectMotion *position;
    StepSequence_StepMovingSpriteEffect(owner);
    position = &owner->sprite.position;
    motion = &owner->motion;
    position->x = position->x + motion->x;
    position->y = position->y + motion->y;
    position->z = position->z + motion->z;
    {
        u32 low = ((u32)(-(((s16)motion->x) >> 4))) << 16;
        u32 high = ((u32)((-(((s16)owner->motion.y) >> 4)) - 8)) << 16;
        asm("" : "+r"(low), "+r"(high));
        delta.packed[0] = (low >> 16) | high;
    }
    delta.bits.z = -(((s16)owner->motion.z) >> 4);
    motion->x = delta.words.x + motion->x;
    motion->y = delta.words.y + motion->y;
    motion->z = delta.words.z + motion->z;
    if ((--owner->lifetime) == 0) {
        owner->update = 0;
        if (owner->onComplete) {
            Callback_InvokeActor(owner, owner->onComplete);
            owner->onComplete = 0;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Nidhoggr_StepMovingSpriteEffect);
#endif
