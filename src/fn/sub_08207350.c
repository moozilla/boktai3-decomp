#include "global.h"
#ifdef NONMATCHING
// fuzzy: 88.0%
// Remaining differences: owner/address allocation, facing update and branch sharing.
// MATCHING_CONCESSION: register-pinning
#define VC_FIRST    "r2"
#define VC_TEMP     "r2"
#define VC_MASK     "r6"
#define VC_DURATION "r4"
#define VC_WEIGHT   "r2"
#include "vulcan_cannon_worker.h"
#define VulcanCannon_UpdateFacingTurn sub_08207350
void sub_08206C00(void);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
static inline u32 TurnAnimation(struct VulcanCannon *source)
{
    register struct VulcanCannon *owner asm("r12") = source;
    struct DisplaySequenceSprite *sprite = &owner->display.sprite;
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
        register u32 first asm(VC_FIRST);
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->display.sprite.flags |= 4;
        else
            owner->display.sprite.flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm(VC_FIRST);
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
        register u32 mask asm(VC_MASK);
        register u32 temp asm(VC_TEMP);
        register u32 savedDuration asm(VC_DURATION);
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
                register s32 weight asm(VC_WEIGHT) = state->frameWeight;
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

void VulcanCannon_UpdateFacingTurn(struct VulcanCannon *owner)
{
    register struct VulcanCannon *p asm("r12") = owner;
    register u32 token asm("r0");
    if (!p->allowTurn) {
        p->state = sub_08206C00;
        Anim_RestartSequence(&p->sequence, p->resource, 0x11, (u8)p->facing, 0);
        token = 0;
        asm("" : "+r"(token));
        return;
    }
    {
        u32 ended;
        token = (u32)owner;
        asm("" : "+r"(token));
        ended = TurnAnimation(p);
        asm("" : : "r"(token));
        if (ended) {
            if (!p->facing)
                p->facing = 1;
            else
                p->facing = 0;
            asm("" : : "r"(p) : "memory");
            p->state = sub_08206C00;
            Anim_RestartSequence(&p->sequence, p->resource, 0x11, (u8)p->facing, 0);
        }
    }
    return;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08207350);
#endif
