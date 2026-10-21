// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "display_animation.h"
#include "contact_actor.h"

struct SunDiscEffectPool {
    u8 unk0[0x34];
    void *sequenceResource;
};

struct SunDiscEffect {
    u32 flags;
    u8 unk4[3];
    u8 mode;
    u8 unk8[8];
    u16 frame;
    u8 unk12[0xa];
    struct ContactPosition position;
    u8 unk24[8];
    struct DisplaySequenceState sequence;
    u8 unk3C[0x58];
    struct ContactPosition colliderPosition;
    u8 unk9C[0x5C];
    u8 resetPending;
    u8 timer;
};

static inline void Step(struct SunDiscEffect *input)
{
    register struct SunDiscEffect *slot asm("r5") = input;
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 ended, duration, flags, bit;
    state = &slot->sequence;
    command = (const volatile u16 *)((u32)slot->sequence.commands + (state->index << 1));
    slot->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
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
                slot->flags |= 4;
            else
                slot->flags &= ~4;
        }
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
            slot->flags |= 8;
        else
            slot->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r6");
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
    return;
}

void SunDiscEffect_Update(struct SunDiscEffectPool *pool, struct SunDiscEffect *effect)
{
    struct SunDiscEffectPool *pl = pool;
    register struct SunDiscEffect *keep asm("r5") = effect;
    u8 *pending = &keep->resetPending;
    if (*pending) {
        keep->colliderPosition = keep->position;
        keep->position.x -= 0x80;
        Anim_SetSequence(&keep->sequence, pl->sequenceResource, 1, 0, 0);
        keep->mode = 2;
        *pending = 0;
    }
    Step(keep);
    {
        u8 *collider = (u8 *)keep + 0x9C;
        Collider_SetPosition(collider, &keep->colliderPosition, 0);
        Collider_Refresh(collider);
    }
    {
        u8 *q = &keep->timer;
        u32 v = *q;
        if (v) {
            v--;
        } else {
            q = (u8 *)keep + 0xF2;
            v = 4;
        }
        *q = v;
    }
}
