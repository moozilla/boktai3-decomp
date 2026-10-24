// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: tentative cue role from callback phases.
// Remaining scale offsets, phase tails and playback register lifetimes differ.

struct P {
    u8 pad0[0x14C];
    struct ContactSwarmSprite sprite;
    u8 pad1[0x3A2 - 0x14C - sizeof(struct ContactSwarmSprite)];
    u8 facing, flip, dir;
    u8 pad2[0x3FC - 0x3A5];
    struct DisplaySequenceState seq;
    u32 unknown40C;
    const void *resource;
    const void *resource2;
    u8 pad3[0x458 - 0x418];
    u8 phase;
    u8 pad4;
    u16 timer;
    u8 pad5[0x4E8 - 0x45C];
    u16 scale;
    u8 pad6[0x58C - 0x4EA];
    void (*fn)(struct P *);
};
void ActorSequence_Begin(struct P *, s32, s32);
void Player_AddControlFlags(struct P *, u32);
void PlayerCue_UpdateHidden(struct P *);
void PlayerCue_CompleteAction(struct P *);
void PlayerCue_EnterStandUpdate(struct P *);
void Player_RestartAnimationFromResource(struct P *);
void Player_UpdateFacingSide(struct P *);
static inline void Step(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st)
{
    struct ContactSwarmSprite *sprite = sp;
    register struct DisplaySequenceState *state asm("r4") = st;
    const volatile u16 *command;
    u32 duration;
    u32 flags;
    u32 bit;
    u32 k1;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        u32 first;
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != ((((*command) & (k1 = 0x30)) >> 4) & bit))
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
        if (first != ((((*command) & k1) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
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
                } else {
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
        }
    }
}

// confidence=low: motion 7 changes sprite scale and visibility over six phases.
inline static void Step_UpdateSideFade(struct ContactSwarmSprite *sp,
                                       struct DisplaySequenceState *st)
{
    struct ContactSwarmSprite *sprite = sp;
    register struct DisplaySequenceState *state asm("r4") = st;
    const volatile u16 *command;
    u32 duration;
    u32 flags;
    u32 bit;
    u32 k1;
    typeof(bit) c1;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        u32 first;
        s8 maskBit = 1;
        bit = maskBit;
        c1 = bit;
        asm("" : "+r"(c1));
        first = c1;
        first &= input;
        if (first != ((((*command) & (k1 = 0x30)) >> 4) & bit))
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
        if (first != ((((*command) & k1) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
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
                } else {
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
        }
    }
}

void PlayerCue_UpdateSideFade(struct P *p)
{
    switch (p->phase) {
        case 0:
            if (!p->timer) {
                p->dir = 4;
                Player_UpdateFacingSide(p);
            }
            Anim_SetSequence(&p->seq, p->resource, 0, p->facing, p->flip);
            Step_UpdateSideFade(&p->sprite, &p->seq);
            if ((++p->timer) > 27)
                ActorSequence_Begin(p, 7, 1);
            break;

        case 1:
            p->sprite.flags |= 2;
            p->sprite.unknown8[0] = 64 - (((u8)p->timer) << 2);
            if (p->timer > 8) {
                p->sprite.unknown8[1] = p->timer << 3;
                p->scale = (p->timer - 8) * 20;
                Player_AddControlFlags(p, 4);
            }
            if ((++p->timer) > 15)
                ActorSequence_Begin(p, 7, 2);
            break;

        case 2:
            Player_AddControlFlags(p, 4);
            p->scale = (p->timer * 200) + 160;
            if ((++p->timer) > 7) {
                Player_AddControlFlags(p, 1);
                ActorSequence_Begin(p, 0, 0);
                p->fn = PlayerCue_UpdateHidden;
                PlayerCue_CompleteAction(p);
            }
            break;

        case 3:
            if (!p->timer) {
                p->dir = 4;
                Player_UpdateFacingSide(p);
            }
            Anim_SetSequence(&p->seq, p->resource, 0, p->facing, p->flip);
            Step_UpdateSideFade(&p->sprite, &p->seq);
            Player_AddControlFlags(p, 4);
            p->sprite.flags |= 2;
            p->sprite.unknown8[0] = 4;
            p->sprite.unknown8[1] = 120;
            p->scale = ((8 - p->timer) * 200) + 160;
            if ((++p->timer) > 7)
                ActorSequence_Begin(p, 7, 4);
            break;

        case 4:
            Anim_SetSequence(&p->seq, p->resource, 0, p->facing, p->flip);
            Step_UpdateSideFade(&p->sprite, &p->seq);
            p->sprite.flags |= 2;
            p->sprite.unknown8[0] = 64 - ((15 - ((u8)p->timer)) << 2);
            if (p->timer <= 7) {
                p->sprite.unknown8[1] = (15 - p->timer) << 3;
                p->scale = (8 - p->timer) * 20;
            } else {
                p->sprite.unknown8[1] = 64;
                p->scale = 0;
                Player_AddControlFlags(p, 4);
            }
            if ((++p->timer) > 15)
                ActorSequence_Begin(p, 7, 5);
            break;

        case 5:
            p->sprite.flags &= ~2;
            Anim_SetSequence(&p->seq, p->resource, 0, p->facing, p->flip);
            Step_UpdateSideFade(&p->sprite, &p->seq);
            if ((++p->timer) > 3) {
                Player_UpdateFacingSide(p);
                PlayerCue_EnterStandUpdate(p);
                p->fn = Player_RestartAnimationFromResource;
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", PlayerCue_UpdateSideFade);
#endif
