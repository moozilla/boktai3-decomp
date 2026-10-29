// MATCHING_CONCESSION: register-pinning
#include "global.h"

#include "display_animation.h"

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
static inline u8 Ooze_MaskedFlags(u8 value, u32 mask)
{
    mask &= value;
    return mask;
}
inline static u32 Ooze_StepAttackAnimation(struct DisplaySequenceSprite *sprite,
                                           struct DisplaySequenceState *state, u32 initialMask)
{
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u32 flags;
    register u32 bit asm("r3");
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->frame = (*command) >> 6;
    {
        u8 input = state->flags;
        register u32 first asm("r1");
        bit = initialMask;
        first = bit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u8 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u8 savedDuration;
        if (++state->elapsed >= (savedDuration = duration = state->duration)) {
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
                u8 frameCount;
                u32 endFlag;
                if (++state->index >= (frameCount = state->frameCount)) {
                    endFlag = Ooze_MaskedFlags(flags, 8);
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
                s32 weight = state->frameWeight;
                s32 speed;
                s32 delay;
                speed = state->speed;
                delay = (speed * weight) >> 6;
                state->duration = delay;
                if (!((u8)delay))
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
            Ooze_StepAttackAnimation(&p->sprite, &p->sequence, 1);
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
            if (Ooze_StepAttackAnimation(&p->sprite, &p->sequence, phase)) {
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
            if (Ooze_StepAttackAnimation(&p->sprite, &p->sequence, 1)) {
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
