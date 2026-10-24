// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// twin.py transplant from a matched near-clone
// Remaining differences: register lifetimes, flag branches and collision-field copies.

#include "contact_swarm.h"

// Drawable layout shared with ActorDisplay_UpdateForegroundSprite.
struct Drawable {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    u32 unknown3C;
    s32 ended;
    u32 mode;
};
static inline u32 StepPlayback(struct Drawable *owner, struct DisplaySequenceState *state)
{
    struct ContactSwarmSprite *sprite = &owner->sprite;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, bit;
    u32 duration;
    u32 flags;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        u32 first;
        u32 maskBit = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
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
        asm("" : "+r"(mask));
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

void Anim_RestartSequence(void *, void *, u16, u8, u8);
void Projectile_SetUpdateCallback(void *, void *);
void Projectile_SetCollisionSize(void *, u16, u16, u16);
u32 sub_08249498(u32, u32);
void Task4E01_BeginImpactAnimation(void *);
void Task4E01_SpawnImpactChildren(void *);
// confidence=low: advances the projectile display and expands collision on every fifteenth tick.
static inline u32 StepPlayback_UpdateExpandingImpact(struct Drawable *owner,
                                                     struct DisplaySequenceState *state)
{
    struct ContactSwarmSprite *sprite = &owner->sprite;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended;
    u32 bit;
    u32 duration;
    u32 flags;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        u32 first;
        u32 maskBit = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
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
        asm("" : "+r"(mask));
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

s32 Task4E01_UpdateExpandingImpact(u8 *p)
{
    u8 *config = p + 0x18;
    struct Drawable *sprite;
    struct DisplaySequenceState *state;
    u32 ended;
    s32 ticks = --(*((s32 *)(config + 8)));
    if (ticks <= 3)
        *((s32 *)(config + 8)) = 4;
    else {
        u32 one = 1;
        *((u32 *)(p + 4)) |= one;
    }
    if ((!((*((u16 *)config)) & 0x10)) ||
        (((*((u32 *)p)) & 3) == (((*((u16 *)config)) & 0xf00) >> 8))) {
        u32 two = 2;
        *((u32 *)(p + 4)) |= two;
    }
    sprite = (void *)(p + 0x8c);
    state = (void *)(p + 0x180);
    ended = StepPlayback_UpdateExpandingImpact(sprite, state);
    if (((ended == 1) && (!((*((u16 *)config)) & 0x10))) && (!(*((u32 *)(p + 0x190))))) {
        *((u32 *)(p + 0x194)) = 4;
        Anim_RestartSequence(state, *((void **)(p + 0x19c)), 4, 0, 0);
        StepPlayback_UpdateExpandingImpact((void *)(p + 0x8c), state);
        ++(*((u32 *)(p + 0x190)));
    }
    {
        u32 tick = *((u32 *)p);
        if ((sub_08249498(tick, 15) == 14) && (tick <= 44)) {
            s32 size = *((s32 *)(p + 0x1a0)) += 16;
            Projectile_SetCollisionSize(config, size, 80, size);
            *((u32 *)(p + 0xd4)) = *((u32 *)(config + 0x28));
            state = (struct DisplaySequenceState *)(p + 0x2EC);
        }
    }
    if ((*((u32 *)p)) >= (*((u16 *)(config + 2)))) {
        if ((*((u16 *)config)) & 0x10)
            Task4E01_BeginImpactAnimation(p);
        else {
            *((u16 *)config) |= 0x40;
            Projectile_SetUpdateCallback(config, Task4E01_SpawnImpactChildren);
        }
    }
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", Task4E01_UpdateExpandingImpact);
#endif
