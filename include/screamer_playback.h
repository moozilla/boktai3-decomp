#ifndef GUARD_SCREAMER_PLAYBACK_H
#define GUARD_SCREAMER_PLAYBACK_H
#include "display_animation.h"
struct ActorPosition {
    s16 x, y, z, pad;
};
struct ActorSprite {
    u32 flags;
    u8 unknown4[0xc];
    u16 frame;
    u8 unknown12[0xa];
    struct ActorPosition position;
    u8 unknown24[8];
};
struct ScreamerState {
    u8 unknown0[24];
    u32 age;
    struct ActorPosition initialPosition;
    struct ActorSprite sprite;
    u8 graphics[0x1c];
    u8 attackCollider[0x54];
    u8 bodyCollider[6];
    u16 colliderFlags;
    u8 unknownC8[82];
    u16 contactFlags;
    u8 unknown11C[76];
    u8 body[0x48];
    void *update;
    struct DisplaySequenceState sequence;
    s16 f1C4;
    s16 f1C6;
    s16 f1C8;
    s16 f1CA;
    s16 f1CC;
    s16 f1CE;
    s16 f1D0;
    s16 f1D2;
    s16 f1D4;
    s16 f1D6;
    s16 f1D8;
    s16 f1DA;
    s16 f1DC;
    s16 f1DE;
    s16 f1E0;
    s16 f1E2;
    s16 f1E4;
    s16 f1E6;
    s16 f1E8;
    s16 f1EA;
    s16 f1EC;
    s16 f1EE;
    s16 f1F0;
    s16 f1F2;
    s16 f1F4;
    s16 f1F6;
    void *f1F8;
    u32 f1FC;
    u32 f200;
    u32 f204;
    u32 f208;
    u32 f20C;
    u32 f210;
    u32 f214;
    void *f218;
    u32 f21C;
};
#ifndef SCREAMER_FIRST
#define SCREAMER_FIRST    "r1"
#define SCREAMER_MASK     "r6"
#define SCREAMER_DURATION "r3"
#define SCREAMER_WEIGHT   "r1"
#endif
static inline u32 Screamer_StepAnimation(struct ScreamerState *owner)
{
    struct ActorSprite *sprite = &owner->sprite;
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
        register u32 first asm(SCREAMER_FIRST);
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
        register u32 first asm(SCREAMER_FIRST);
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
        register u32 mask asm(SCREAMER_MASK);
        register u32 temp asm("r1");
        register u32 savedDuration asm(SCREAMER_DURATION);
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
                register s32 weight asm(SCREAMER_WEIGHT) = state->frameWeight;
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

#endif
