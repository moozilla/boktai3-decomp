// MATCHING_CONCESSION: register-pinning
#include "global.h"

#include "display_animation.h"

#ifdef NONMATCHING
// fuzzy: 91.6%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Transition constants, mask copies, and register allocation still differ.

struct Ooze {
    u8 pad0[0x20];
    struct DisplaySequenceSprite sprite;
    u8 pad4c[0x104 - 0x4c];
    void (*state)(void);
    struct DisplaySequenceState sequence;
    s16 startDelay, unknown11a, unknown11c, timer;
    u8 pad120[0x18];
    s16 repeatAttack, repeatLimit, repeatCount, attackDelay, phase, unknown142;
    u8 facing, unknown145;
    u16 idleSequence, startSequence, holdSequence, endSequence;
    u8 pad14e[0xA];
    void *resource;
};
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
void Ooze_UpdateAttackDelay(void);
static inline u32 Ooze_StepAttackAnimation(struct DisplaySequenceSprite *sprite,
                                           struct DisplaySequenceState *state, u32 initialMask)
{
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u8 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = initialMask;
        bit = maskBit;
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
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
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
        u32 temp;
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        asm("" : "+&r"(mask) : "r"(temp));
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
                command = (const volatile u16 *)(((u32)table) + offset);
            }
            state->frameWeight = (*command) & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!((u8)speed))
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}

inline static u32 Ooze_StepAttackAnimation_BeginAttack(struct DisplaySequenceSprite *sprite,
                                                       struct DisplaySequenceState *state,
                                                       u32 initialMask)
{
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u8 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = initialMask;
        bit = maskBit;
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
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
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
        u32 temp;
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        asm("" : "+&r"(mask) : "r"(temp));
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
                u8 endFlag;
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
                command = (const volatile u16 *)(((u32)table) + offset);
            }
            state->frameWeight = (*command) & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!((u8)speed))
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}

s32 Ooze_BeginAttack(struct Ooze *p)
{
    register s32 phase asm("r3") = p->phase;
    switch (phase) {
        case 0:
            Ooze_StepAttackAnimation_BeginAttack(&p->sprite, &p->sequence, 1);
            if (p->timer > p->attackDelay) {
                if (p->repeatAttack)
                    Anim_RestartSequence(&p->sequence, p->resource, p->startSequence, p->facing, 0);
                else
                    Anim_RestartSequence(&p->sequence, p->resource, p->holdSequence, p->facing, 8);
                p->phase = 1;
                Sound_PlaySong(0x523);
            }
            break;

        case 1: {
            void *collider = ((u8 *)p) + 0x68;
            if (Ooze_StepAttackAnimation_BeginAttack(&p->sprite, &p->sequence, phase)) {
                if (p->repeatAttack) {
                    if ((++p->repeatCount) >= p->repeatLimit) {
                        Anim_RestartSequence(&p->sequence, p->resource, p->endSequence, p->facing,
                                             0);
                        p->phase = 2;
                    }
                } else {
                    Anim_RestartSequence(&p->sequence, p->resource, p->endSequence, p->facing, 0);
                    p->phase = 2;
                }
            }
            Collider_SetPosition(collider, ((u8 *)p) + 0x3c, 0);
            Collider_Refresh(collider);
        } break;

        case 2:
            if (Ooze_StepAttackAnimation_BeginAttack(&p->sprite, &p->sequence, 1)) {
                Anim_RestartSequence(&p->sequence, p->resource, p->idleSequence, p->facing, 8);
                p->state = Ooze_UpdateAttackDelay;
                p->phase = 0;
                p->timer = 0;
                p->repeatCount = 0;
            }
            break;
    }

    p->sprite.flags &= ~1;
    p->timer++;
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", Ooze_BeginAttack);
#endif
