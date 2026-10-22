// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 96.6%
// Remaining differences: timer/count operand allocation and derived restart-sequence offsets.
#include "screamer_playback.h"
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Anim_SetDurationScale(void *, u32);
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
s32 Screamer_WaitForPlayerApproach(struct ScreamerState *);
static inline u32 Screamer_StepScreamLoop(struct ScreamerState *owner, u32 initialBit)
{
    struct ScreamerSprite *sprite = &owner->sprite;
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
        u32 maskBit = initialBit;
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
        register u32 temp asm("r2");
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

// The recovery branch loads its duration mask through r3.
static inline u32 Screamer_StepRecovery(struct ScreamerState *owner)
{
    struct ScreamerSprite *sprite = &owner->sprite;
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
        register u32 temp asm("r3");
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

s32 Screamer_UpdateScream(struct ScreamerState *p)
{
    register s32 phase asm("r3");
    p->sprite.flags &= ~1;
    phase = p->f1D6;
    asm("" : "+r"(phase));
    switch (phase) {
        case 0:
            Screamer_StepAnimation(p);
            if (({
                    register u32 off asm("r2") = 0x1ca;
                    asm("" : "+r"(off));
                    *(s16 *)((u8 *)p + off);
                }) > ({
                    register u32 off asm("r3") = 0x1d4;
                    asm("" : "+r"(off));
                    *(s16 *)((u8 *)p + off);
                })) {
                Anim_RestartSequence(&p->sequence, p->f218, 0x1B, (u8)p->f1D8, 0);
                Anim_SetDurationScale(&p->sequence, 0x20);
                p->f1D6 = 1;
                Sound_PlaySong(0x51A);
            }
            p->colliderFlags &= ~4;
            Collider_SetPosition(p->bodyCollider, &p->sprite.position, 0);
            p->contactFlags |= 4;
            break;
        case 1:
            if (Screamer_StepScreamLoop(p, phase)) {
                {
                    u32 next = p->f1D2 + 1;
                    p->f1D2 = next;
                    if ((s16)next >= p->f1D0) {
                        Anim_RestartSequence(&p->sequence, p->f218, 0x1D, (u8)p->f1D8, 0);
                        p->f1D6 = 2;
                    }
                }
            }
            Collider_SetPosition(p->attackCollider, &p->sprite.position, 0);
            Collider_Refresh(p->attackCollider);
            p->colliderFlags &= ~4;
            p->contactFlags |= 4;
            break;
        case 2:
            Screamer_StepRecovery(p);
            if (p->f1CA > p->f1CC) {
                Anim_RestartSequence(&p->sequence, p->f218, 0x1A, (u8)p->f1D8, 0);
                p->update = Screamer_WaitForPlayerApproach;
                p->f1D6 = 0;
                p->f1CA = 0;
                p->f1D2 = 0;
            }
            p->colliderFlags &= ~4;
            Collider_SetPosition(p->bodyCollider, &p->sprite.position, 0);
            p->contactFlags |= 4;
            break;
    }
    p->sprite.flags &= ~1;
    p->f1CA++;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", Screamer_UpdateScream);
#endif
