// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "render.h"
// confidence=low: tentative player cue role; see callback body.
// Signed scaling sign tests and direction-store layout still differ.
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
static inline s32 Trunc6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -((-v) >> 6);
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

#define U8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define U16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S16(p, o) (*(s16 *)((u8 *)(p) + (o)))
void Player_RestartAnimationFromResource(struct P *);
void Player_UpdateFacingSide(struct P *);
u32 Player_SetAnimation(struct P *, u32, u32, u32);
void SpiralTrail_CreateWithParticles(struct P *, s32, void *, s32, s32);
// confidence=low: state-four variant uses eight-facing animation and axis-dependent speed.
void PlayerCue_UpdateSideWalkTarget(struct P *p)
{
    s32 dx = p->tx - p->x, dy = p->ty - p->y;
    s32 angle = Math_Atan2(dx, dy);
    if (dx * dx + dy * dy <= 144) {
        p->x = p->tx;
        p->y = p->ty;
        if (p->finalDir >= 0) {
            p->dir = p->finalDir;
            p->finalDir = -1;
        } else {
            u32 d;
            register s32 t asm("r0");
            register u8 *q;
            t = (u16)angle + 112;
            asm("" : "+r"(t));
            t >>= 5;
            d = t & 7;
            q = &p->dir;
            *q = d;
        }
        if (p->dir > 4) {
            p->facing = 8 - p->dir;
            p->flip = 1;
        } else {
            p->facing = p->dir;
            p->flip = 0;
        }
        Anim_SetSequence(&p->seq, *(void **)((u8 *)p + 0x410), 0, p->facing, p->flip);
        Step(&p->sprite, &p->seq);
        ActorSequence_Begin(p, 0, 0);
        {
            register u32 off asm("r2") = 0x58C;
            register u8 *q asm("r1");
            asm("" : "+r"(off));
            q = (u8 *)p + off;
            *(void **)q = Player_RestartAnimationFromResource;
        }
        PlayerCue_CompleteAction(p);
    } else {
        p->vx = Truncate(gUnk_086149C4[(angle + 64) & 255] * 12);
        p->vy = Truncate(gUnk_086149C4[angle & 255] * 12);
        if (U8(p, 0x28F) == 1) {
            s32 v;
            if (p->vy < 0) {
                v = p->vy * 40;
                p->vy = Trunc6(v);
            } else {
                v = p->vy * 52;
                p->vy = Trunc6(v);
            }
        } else if (U8(p, 0x28F) == 2) {
            s32 v;
            if (p->vx < 0) {
                v = p->vx * 40;
                p->vx = Trunc6(v);
            } else {
                v = p->vx * 52;
                p->vx = Trunc6(v);
            }
        }
        {
            u32 dir;
            register u32 d2 asm("r1");
            register s32 t asm("r0");
            t = (u16)angle + 112;
            asm("" : "+r"(t));
            t >>= 5;
            d2 = t & 7;
            {
                u8 *q = &p->dir;
                dir = d2;
                *q = d2;
            }
            if (dir > 4) {
                p->facing = 8 - dir;
                p->flip = 1;
            } else {
                register u32 z asm("r0");
                u8 *fq;
                p->facing = d2;
                fq = &p->flip;
                z = 0;
                *fq = z;
            }
        }
        Anim_SetSequence(&p->seq, *(void **)((u8 *)p + 0x410), 1, p->facing, p->flip);
        Step(&p->sprite, &p->seq);
    }
}
