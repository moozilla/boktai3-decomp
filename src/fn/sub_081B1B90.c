#include "global.h"
// fix.py best version (reprinted C)
// Animation setup and tail scheduling still differ.
// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: provisional role inferred from state transitions and player-motion calls

// confidence=low: approaches a side of the player before playing sequence eight

#include "display_animation.h"
#include "scene_vec.h"
struct ActorStatusPart {
    u32 spriteFlags;
    u8 unknown4[0xc];
    u16 frame;
    u8 unknown12[0xa];
    struct SceneVec position;
    u8 unknown24[0x24];
    struct DisplaySequenceState sequence;
    u8 motion[0x38];
    struct SceneVec *anchor;
    u8 unknown94[8];
    struct SceneVec offset;
    u8 unknownA4[0xb8];
    u32 variant, flags, state, timer;
    u8 action, previousAction, reset, enter, finished;
    u8 unknown171[0x2b];
    u32 countdown;
};
void MotionTrack_Update(void *);
struct SceneVec *Player_GetEffectivePosition(u32);
void OrbitMotion_BeginDistanceWave(void *, const void *, const void *, u32, u32);
static inline u32 StepPlayback(struct ActorStatusPart *owner, struct DisplaySequenceState *sequence,
                               struct ActorStatusPart *base, u32 one)
{
    struct DisplaySequenceState *state = sequence;
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    typeof(bit) c1;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = base->sequence.commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    owner->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        u32 first;
        u32 maskBit = one;
        bit = maskBit;
        c1 = bit;
        asm("" : "+r"(c1));
        first = c1;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            owner->spriteFlags |= 4;
        else
            owner->spriteFlags &= ~4;
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
            owner->spriteFlags |= 8;
        else
            owner->spriteFlags &= ~8;
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

static inline void Difference(struct SceneVec *out, struct SceneVec *a, struct SceneVec *b)
{
    out->x = (u16)a->x - (u16)b->x;
    out->y = (u16)a->y - (u16)b->y;
    out->z = (u16)a->z - (u16)b->z;
}
static inline u8 MotionEnded(struct ActorStatusPart *p)
{
    u8 result = 0;
    if (p->motion[0x29] == 2)
        result = 1;
    return result;
}
void ActorStatus_UpdateFlankingStrike(u8 *boss, struct ActorStatusPart *p)
{
    struct SceneVec target;
    u8 *enter;
    register u32 state asm("r7");
    u32 mask = 8;
    typeof(state) c1;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        p->state = 0;
        p->timer = 0;
        p->enter = 1;
    }
    state = p->state;
    c1 = state;
    asm("" : "+r"(state));
    switch (c1) {
        case 0: {
            register u32 entered asm("r1");
            {
                register u8 *address asm("r0") = &p->enter;
                entered = *address;
                asm("" : "+r"(address), "+r"(entered));
                enter = address;
                asm("" : "+r"(enter));
            }
            if (entered) {
                register u32 zero asm("r0") = 0;
                asm("" : "+r"(zero));
                *enter = zero;
                Difference(&target, Player_GetEffectivePosition(zero), p->anchor);
                if (p->variant == 1) {
                    target.x += 512;
                    target.z -= 512;
                } else {
                    target.x -= 512;
                    target.z += 512;
                }
                OrbitMotion_BeginDistanceWave(p->motion, &p->offset, &target,
                                              ((p->variant * 128) + 96) & 255, 4);
            }
            MotionTrack_Update(p->motion);
            if (MotionEnded(p)) {
                u32 next = 1;
                asm("" : "+r"(next));
                p->state = next;
                p->timer = 0;
                *enter = next;
            }
            break;
        }

        case 1: {
            register struct DisplaySequenceState *seq asm("r4");
            register u32 entered asm("r1");
            {
                register u8 *address asm("r0") = &p->enter;
                entered = *address;
                asm("" : "+r"(address), "+r"(entered));
                enter = address;
                asm("" : "+r"(enter));
            }
            seq = &p->sequence;
            if (entered) {
                register u32 zero asm("r2") = 0;
                asm("" : "+r"(zero));
                *enter = zero;
                Anim_SetSequence(seq, *((void **)(boss + 0x4d0)), 8, (u8)p->variant, zero);
            }
            if (StepPlayback(p, seq, p, c1)) {
                {
                    register u32 next asm("r1") = 2;
                    asm("" : "+r"(next));
                    p->state = next;
                }
                p->timer = 0;
                *enter = 1;
            }
            break;
        }

        case 2:
            p->finished = 1;
            break;
    }
}
