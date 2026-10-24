// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
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
void ActorSequence_Begin(struct P *, s32, s32);
void PlayerCue_CompleteAction(struct P *);
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

#include "scene_vec.h"
static inline void Face(struct P *p, u32 phase)
{
    if (p->dir > 4) {
        p->facing = (8 - p->dir) >> 1;
        p->flip = 1;
    } else {
        p->facing = p->dir >> 1;
        p->flip = phase;
    }
}
static inline u32 StepEnd(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st, u32 phase)
{
    struct ContactSwarmSprite *sprite = sp;
    struct DisplaySequenceState *state = st;
    const volatile u16 *command;
    u32 duration, flags, bit;
    u32 ended;
    u32 finalIndex;
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
        u32 maskBit = phase;
        bit = maskBit;
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
                        goto ended_animation;
                    } else {
                        finalIndex = state->frameCount - 1;
                        goto store_final_index;
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
                    finalIndex = endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        finalIndex = frameCount - 1;
                    }
                    goto store_final_index;
                } else {
                }
            }
            goto not_ended;
        store_final_index:
            state->index = finalIndex;
        ended_animation:
            ended = 1;
            goto update_frame;
        not_ended:
            ended = 0;
        update_frame: {
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
// confidence=low: reduces departure scale and returns to standing after animation.
void PlayerCue_UpdateSideArrival(struct P *p)
{
    u32 phase = p->phase;
    switch ((s16)phase) {
        case 0:
            if (!p->timer) {
                Face(p, phase);
                ++p->timer;
            }
            Anim_SetSequence(&p->seq, p->resource, 2, p->facing, p->flip);
            Step(&p->sprite, &p->seq);
            if (p->scale > 30)
                p->scale -= 30;
            else {
                p->scale = 0;
                Sound_PlaySong(0x1B0);
                ActorSequence_Begin(p, 0x1C, 1);
            }
            break;
        case 1:
            Anim_SetSequence(&p->seq, p->resource, 3, p->facing, p->flip);
            if (StepEnd(&p->sprite, &p->seq, phase)) {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 0, 0);
                {
                    register u32 callbackOffset asm("r2") = (u32) & ((struct P *)0)->fn;
                    register void (**callback)(struct P *) asm("r1");
                    asm("" : "+r"(callbackOffset));
                    callback = (void (**)(struct P *))((u32)p + callbackOffset);
                    *callback = Player_RestartAnimationFromResource;
                }
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
