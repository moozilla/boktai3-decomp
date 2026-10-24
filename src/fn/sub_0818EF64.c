// MATCHING_CONCESSION: register-pinning
#include "global.h"

// Exact with register-pinning and empty-asm concessions.
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
        u8 savedDuration;
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
                    endFlag = 8;
                    endFlag &= flags;
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
                s32 weight = state->frameWeight;
                u16 speed = state->speed;
                s32 ticks = (speed * weight) >> 6;
                state->duration = ticks;
                if (!(u8)ticks)
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
static inline void AddMotion(struct NidhoggrEffectMotion *motion,
                             const struct NidhoggrEffectMotion *delta)
{
    motion->x += delta->x;
    motion->y += delta->y;
    motion->z += delta->z;
}

void Nidhoggr_StepMovingSpriteEffect(struct NidhoggrAnimatedSlot *ownerArg)
{
    struct NidhoggrAnimatedSlot *owner = ownerArg;
    struct EffectDelta delta;
    struct NidhoggrEffectMotion *motion;
    struct NidhoggrEffectMotion *position;
    StepSequence(owner);
    position = &owner->sprite.position;
    motion = &owner->motion;
    position->x = position->x + motion->x;
    position->y = position->y + motion->y;
    position->z = position->z + motion->z;
    delta.x = -(((s16)motion->x) >> 4);
    delta.y = -(((s16)owner->motion.y) >> 4) - 8;
    delta.z = -(((s16)owner->motion.z) >> 4);
    AddMotion(motion, (struct NidhoggrEffectMotion *)&delta);
    if ((--owner->lifetime) == 0) {
        owner->update = 0;
        if (owner->onComplete) {
            Callback_InvokeActor(owner, owner->onComplete);
            owner->onComplete = 0;
        }
    }
}
