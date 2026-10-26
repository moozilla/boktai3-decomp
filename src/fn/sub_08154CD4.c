// confidence=low: inferred from player cue phases and animation consumers.
// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "render.h"
// confidence=low: tentative cue role from callback phases.

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
void Player_AddControlFlags(struct P *, u32);
void PlayerCue_EnterStandUpdate(struct P *);
static inline void Step(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st)
{
    struct ContactSwarmSprite *sprite = sp;
    struct DisplaySequenceState *state = st;
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
        u32 input = state->flags;
        register u32 first asm("r1");
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
        register u32 first asm("r1");
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

void SpiralTrail_CreateWithParticles(struct P *, s32, s32, s32, s32);
void Player_SetAnimation(struct P *, s32, s32, s32);
#define B(o) (*(u8 *)((u8 *)p + (o)))
#define H(o) (*(u16 *)((u8 *)p + (o)))
#define W(o) (*(u32 *)((u8 *)p + (o)))

// confidence=low: inferred from the side-facing cue callbacks and effects.
void PlayerCue_UpdateSpiralRecovery(struct P *p)
{
    if (!p->timer) {
        H(0xAD8) = 0;
        Sound_PlaySong(0x37F);
        SpiralTrail_CreateWithParticles(p, 1, 0, 32, 56);
    }
    {
        u32 timer = p->timer;
        register u32 seven asm("r1") = 7;
        register u32 low asm("r2") = seven;
        low &= timer;
        if ((s32)low >= (s32)(seven - (timer >> 2))) {
            Player_AddControlFlags(p, 0x41);
            *(struct DisplayPosition *)((u8 *)p + 0xAC) =
                *(struct DisplayPosition *)((u8 *)p + 0x30);
        }
    }
    Player_SetAnimation(p, 0x9A, 0x40, 1);
    {
        u16 *target = (u16 *)((u8 *)p + 0xC6);
        u32 resource;
        resource = 0x16F;
        *target = resource;
        W(0xD4) = (u32)(gPaletteBankData + (resource << 5));
    }
    {
        register u32 offset asm("r3") = 0x3FC;
        struct DisplaySequenceState *seq;
        asm("" : "+r"(offset));
        seq = (struct DisplaySequenceState *)((u8 *)p + offset);
        Anim_SetSequence(seq, p->resource, 0, p->facing, p->flip);
        {
            register u32 so asm("r3") = 0x14C;
            asm("" : "+r"(so));
            Step((struct ContactSwarmSprite *)((u8 *)p + so), seq);
        }
    }
    {
        register u32 toff asm("r2") = 0xAD6;
        register u16 *target asm("r1");
        u32 resource;
        asm("" : "+r"(toff));
        target = (u16 *)((u8 *)p + toff);
        resource = 0x16F;
        *target = resource;
    }
    {
        u32 elapsed;
        register u32 mask asm("r2");
        register u32 temp asm("r3");
        register u32 off asm("r3") = 0xAD8;
        u16 *q;
        asm("" : "+r"(off));
        q = (u16 *)((u8 *)p + off);
        elapsed = *q + 3;
        *q = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        if ((u16)elapsed > 32)
            *q = 32;
        elapsed = p->timer + 1;
        p->timer = elapsed;
        if ((elapsed & mask) > 31) {
            B(0x464) = 1;
            B(0x465) = 1;
            PlayerCue_EnterStandUpdate(p);
            {
                register u32 off asm("r3") = 0x5A3;
                u8 *target;
                target = (u8 *)p + off;
                asm("" : "+r"(target), "+r"(off));
                *target = 1;
            }
        }
    }
}
