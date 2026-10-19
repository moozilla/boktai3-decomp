
#ifdef NONMATCHING
// fuzzy: 85.9%
// foundry draft (worker best 85.1%); what differs is mostly register allocation and instruction order
#define Nidhoggr_StepPoolAnimation sub_0818EE38
#include "global.h"

struct NidhoggrPoolSequence {
    const volatile u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct NidhoggrPoolSprite {
    u32 flags;
    u8 unknown4[12];
    u16 frame;
    u8 unknown12[0x1a];
};
struct NidhoggrAnimatedSlot {
    void *update, *onComplete;
    u8 unknown8[0x1c];
    struct NidhoggrPoolSprite sprite;
    struct NidhoggrPoolSequence sequence;
};
s32 Callback_InvokeActor(void *, void *);

void sub_0818EE38(struct NidhoggrAnimatedSlot *owner)
{
    struct NidhoggrPoolSprite *sprite = &owner->sprite;
    struct NidhoggrPoolSequence *state = &owner->sequence;
    const volatile u16 *command = state->commands + state->index;
    u32 ended, duration, flags, bit;
    sprite->frame = *command >> 6;
    {
        u8 input = state->flags;
        u16 first;
        u16 one = 1;
        bit = one;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u8 input = state->flags;
        u16 bit = 2;
        u16 first = (u8)(bit & input);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask = 0xFFFF;
        u32 savedDuration;
        state->elapsed = elapsed;
        duration = state->duration;
        elapsed &= mask;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    if (flags & 8)
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
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = (u8)(flags & 8);
                    if (endFlag) {
                        state->elapsed = duration;
                        state->index = frameCount - 1;
                    } else
                        state->index = 0;
                    ended = 1;
                } else
                    ended = 0;
            }
            command = state->commands + state->index;
            state->frameWeight = *command & 15;
            {
                s32 weight = state->frameWeight;
                s32 speed = state->speed;
                speed = (weight * speed) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else
            ended = 0;
    }
    if (ended) {
        owner->update = 0;
        if (owner->onComplete) {
            Callback_InvokeActor(owner, owner->onComplete);
            owner->onComplete = 0;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0818EE38);
#endif
