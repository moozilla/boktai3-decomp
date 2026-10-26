// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "render.h"

#ifdef NONMATCHING
// fuzzy: 94.1%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: tentative player cue role; see callback body.
// Constant derivation, resource pointer setup and timer mask lifetimes differ.
struct P {
    u8 pad0[0x30];
    s16 x, xf, y, yf, vx, vxf, vy, vyf;
    u8 padpos[0x14C - 0x40];
    struct ContactSwarmSprite sprite;
    u8 pad1[0x3A2 - 0x14C - sizeof(struct ContactSwarmSprite)];
    u8 facing, flip, dir;
    u8 pad2[0x3FC - 0x3A5];
    struct DisplaySequenceState seq;
    const void *resource;
    u8 pad3[0x458 - 0x410];
    u8 phase;
    u8 pad4;
    u16 timer;
    u8 pad5[0x4E8 - 0x45C];
    u16 scale;
    u8 pad6[0x58C - 0x4EA];
    void (*fn)(struct P *);
    s16 tx, txf, ty, tyf;
    u16 speed;
    u8 pad7[3];
    s8 finalDir;
};
void ActorSequence_Begin(struct P *, s32, s32);
void Player_AddControlFlags(struct P *, u32);
void PlayerCue_UpdateHidden(struct P *);
void PlayerCue_CompleteAction(struct P *);
void PlayerCue_EnterStandUpdate(struct P *);
void Player_RestartAnimationFromSetup(struct P *);
extern const s16 gUnk_086149C4[];
void Sound_PlaySong(u32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline void Step(struct ContactSwarmSprite *sprite, struct DisplaySequenceState *state)
{
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

#define U8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define U16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S16(p, o) (*(s16 *)((u8 *)(p) + (o)))
void Player_RestartAnimationFromResource(struct P *);
void Player_UpdateFacingSide(struct P *);
u32 Player_SetAnimation(struct P *, u32, u32, u32);
void SpiralTrail_CreateWithParticles(struct P *, s32, void *, s32, s32);
// confidence=low: motion 13 alternates facing while growing a spiral trail.
static inline void Step_UpdateSpiralMotion(struct ContactSwarmSprite *sprite,
                                           struct DisplaySequenceState *state)
{
    const volatile u16 *command;
    u32 duration;
    u32 flags;
    u32 bit;
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

void PlayerCue_UpdateSpiralMotion(struct P *p)
{
    if (p->timer == 0) {
        if (p->flip)
            p->dir = 7 - (p->facing * 2);
        else
            p->dir = (p->facing * 2) + 1;
        *((u16 *)(((u8 *)p) + 0xAD8)) = 0;
        Sound_PlaySong(0x37F);
        SpiralTrail_CreateWithParticles(p, 1, 0, 32, 56);
    }
    if ((p->timer & 7) < (7 - (p->timer >> 2))) {
        if (p->dir > 3) {
            p->facing = (7 - p->dir) >> 1;
            p->flip = 1;
        } else {
            p->facing = p->dir >> 1;
            p->flip = 0;
        }
    } else {
        Player_AddControlFlags(p, 0x41);
        {
            struct PositionPair {
                u32 x;
                u32 y;
            };
            *((struct PositionPair *)(((u8 *)p) + 0xAC)) =
                *((struct PositionPair *)(((u8 *)p) + 0x30));
        }
        if (p->dir > 4) {
            p->facing = 8 - p->dir;
            p->flip = 1;
        } else {
            p->facing = p->dir;
            p->flip = 0;
        }
    }
    Player_SetAnimation(p, 0x9A, 64, 1);
    {
        u32 resource = 0x16F;
        *((u16 *)(((u8 *)p) + 0xC6)) = resource;
        asm("" : "+r"(resource));
        *((void **)(((u8 *)p) + 0xD4)) = gPaletteBankData + (resource << 5);
    }
    Anim_SetSequence(&p->seq, (void *)p->resource, 0, p->facing, p->flip);
    Step_UpdateSpiralMotion(&p->sprite, &p->seq);
    *((u16 *)(((u8 *)p) + 0xAD8)) += 3;
    {
        u16 k1 = 0x2DD;
        {
            u16 *d2 = (u16 *)(((u8 *)p) + 0xAD4);
            d2[1] = k1;
        }
    }
    {
        u16 *d1 = (u16 *)(((u8 *)p) + 0xAD4);
        if (d1[2] > 32)
            *((u16 *)(((u8 *)p) + 0xAD8)) = 32;
    }
    if ((++p->timer) > 31) {
        *((u8 *)(((u8 *)p) + 0x464)) = 1;
        *((u8 *)(((u8 *)p) + 0x465)) = 1;
        Player_UpdateFacingSide(p);
        PlayerCue_EnterStandUpdate(p);
        *((u8 *)(((u8 *)p) + 0x5A3)) = 1;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", PlayerCue_UpdateSpiralMotion);
#endif
