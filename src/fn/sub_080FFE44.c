// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "display_animation.h"

struct WoodenCrateStep {
    u32 flags;
    u8 unk4[3];
    u8 mode;
    u8 unk8[8];
    u16 frame;
    u8 unk12[0x1A];
    struct DisplaySequenceState sequence;
    u8 unk3C[0x96];
    u16 counterD2;
    u8 resetPending;
};
void WoodenCrate_UpdatePaletteTimers(void *);
void WoodenCrate_CheckTrigger(void *);
void WoodenCrate_UpdateShake(void *);

static inline void Step(struct WoodenCrateStep *input)
{
    register struct WoodenCrateStep *slot asm("r5") = input;
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

#define WoodenCrate_Update sub_080FFE44
void WoodenCrate_Update(void *unused, struct WoodenCrateStep *crate)
{
    register struct WoodenCrateStep *keep asm("r5") = crate;
    u8 *pending = &keep->resetPending;
    u8 zero = 0;
    u16 half = 0;
    if (*pending) {
        u16 *counter = &crate->counterD2;
        *counter = half;
        keep->flags &= ~1;
        *pending = zero;
    }
    Step(keep);
    WoodenCrate_UpdatePaletteTimers(keep);
    WoodenCrate_CheckTrigger(keep);
    WoodenCrate_UpdateShake(keep);
    {
        register u16 *counter asm("r1") = &keep->counterD2;
        *counter = *counter + 1;
    }
}
