// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
struct Fragment {
    u32 flags;
    u8 active, unk_05[3];
    s8 scaleX, scaleY;
    u8 unk_0A[2];
    void *graphics;
    u16 frame;
    u8 unk_12[10];
    struct ContactPosition position;
    u8 unk_24[8];
    struct DisplaySequenceState sequence;
    u8 collider[0x54];
    struct ContactPosition center, offset;
    u32 angle, angularSpeed;
    s16 direction;
    u16 unk_AA;
    s16 radius, scaleTimer;
    u32 unk_B0;
    s32 lifetime;
    u16 unk_B8;
    u8 state;
    s8 slot;
    u16 timer;
    u8 entered;
};
struct Pool {
    u8 unk_00[0x38];
    u32 used;
};
s32 Mod(s32, s32);
s32 Div(s32, s32);
void FragmentEffect_SpawnRandomParticle(struct ContactPosition *);
void Sprite_Remove(void *);
static inline void FragmentEffect_StepAnimation(struct Fragment *sprite)
{
    struct DisplaySequenceState *state = &sprite->sequence;
    const volatile u16 *command;
    u32 duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u8 input = state->flags;
        u8 first;
        u32 maskBit = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u8 input = state->flags;
        u32 bit;
        u8 first;
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        u32 temp;
        u8 savedDuration;
        state->elapsed = elapsed;
        temp = 0xFFFF;
        mask = temp;
        duration = state->duration;
        elapsed = (u16)(elapsed & mask);
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
        }
    }
}
#ifdef NONMATCHING
// fuzzy: 53.8%
// scope shape 301 (differences once low registers are set aside)
// Tentative: shrink the fragment, then release its sprite and pool slot.
void FragmentEffect_ShrinkAndRemove(struct Pool *pool, struct Fragment *p)
{
    s16 *scaleTimer;
    u16 *timer;
    if (p->entered) {
        p->flags = (p->flags & ~1) | 2;
        p->scaleTimer = 20;
        p->timer = 0;
        p->entered = 0;
        p->scaleX = 64;
        p->scaleY = 64;
        p->timer = 0;
    }
    {
        u16 *frame = &p->timer;
        s32 remainder = Mod(*frame, 6);
        timer = frame;
        if (!remainder)
            FragmentEffect_SpawnRandomParticle(&p->position);
    }
    {
        s16 *initial = &p->scaleTimer;
        s32 remaining = *initial;
        scaleTimer = initial;
        if (remaining > 0) {
            p->scaleX += Div(1 - p->scaleX, *scaleTimer);
            p->scaleY += Div(1 - p->scaleY, *scaleTimer);
            (*scaleTimer)--;
        }
    }
    FragmentEffect_StepAnimation(p);
    if (*scaleTimer <= 0) {
        Sprite_Remove(p);
        pool->used &= ~(1 << p->slot);
        p->slot = -1;
        p->state = 0;
        p->entered = 0;
    } else {
        (*timer)++;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", FragmentEffect_ShrinkAndRemove);
#endif
