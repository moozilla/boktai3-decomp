// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
// confidence=low: tentative player cue role; see callback body.

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

extern const s16 gUnk_086149C4[];
void SpiralTrail_CreateWithParticles(struct P *, s32, s32, s32, s32);
void Player_SetAnimation(struct P *, s32, s32, s32);
#define B(o) (*(u8 *)((u8 *)p + (o)))
#define H(o) (*(u16 *)((u8 *)p + (o)))
#define W(o) (*(u32 *)((u8 *)p + (o)))
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

#include "scene_vec.h"
#include "render.h"
void Camera_SnapToWithoutCompletion(struct SceneVec *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
static inline void Face(struct P *p)
{
    if (p->dir > 4) {
        p->facing = (8 - p->dir) >> 1;
        p->flip = 1;
    } else {
        p->facing = p->dir >> 1;
        p->flip = 0;
    }
}
// confidence=low: camera lift and scale descent precede hidden cue state.
void PlayerCue_UpdateSideDeparture(struct P *p)
{
    switch (p->phase) {
        case 0: {
            struct SceneVec pos;
            Player_AddControlFlags(p, 4);
            pos = *(struct SceneVec *)((u8 *)p + 0x30);
            pos.y += 150;
            Camera_SnapToWithoutCompletion(&pos);
            if (B(0x4E0) <= 1) {
                if (B(0x4E0))
                    ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 16);
                else
                    ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 1);
            }
            Face(p);
            Anim_SetSequence(&p->seq, p->resource, 2, p->facing, p->flip);
            Step(&p->sprite, &p->seq);
            Sound_PlaySong(0x1AD);
            ActorSequence_Begin(p, 0x1B, 1);
            p->scale = 0;
            break;
        }
        case 1:
            Player_AddControlFlags(p, 4);
            if (++p->timer > 5) {
                p->scale = 0;
                B(0x153) = 1;
                Sound_PlaySong(0x1AE);
                ActorSequence_Begin(p, 0x1B, 2);
            }
            break;
        case 2:
            Player_AddControlFlags(p, 4);
            p->scale -= 60;
            if (++p->timer > 29) {
                Player_AddControlFlags(p, 1);
                ActorSequence_Begin(p, 0x1B, 3);
                PlayerCue_CompleteAction(p);
            }
            break;
        case 3:
            Player_AddControlFlags(p, 5);
            break;
    }
}
