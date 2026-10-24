// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"
// Matched with tools/scope by gpt-6.1-sol workers; the pins and empty asm barriers are
// temporary concessions.
// confidence=low: moves the part along its stored retreat angle, then returns to idle.

struct VanargandPart {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x1c];
    struct DisplaySequenceState sequence;
    u8 motion[0x38];
    struct SceneVec *basePosition;
    u8 unknown94[8];
    struct SceneVec origin;
    struct SceneVec target;
    s16 heading, distance, particleAngle;
    u8 unknownB2[0xaa];
    u32 variant, flags, state, timer;
    u8 currentAction, previousAction, reset, enter, finished;
    u8 unknown171[7];
    u32 actionTimer;
    u8 unknown17C[0x20];
    u32 flashTimer;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);
extern const s16 gUnk_086149C4[];
static inline s32 ScaleSequenceSpeed(s32 weight, s32 speed)
{
    return (speed * weight) >> 6;
}

static inline void StepSequence(void *obj, u32 firstMask, int preserveMask)
{
    struct VanargandPart *owner = obj;
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 duration, flags, bit;
    state = &owner->sequence;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = owner->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    owner->sprite.frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        bit = firstMask;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
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
            ((struct VanargandPart *)obj)->sprite.flags |= 8;
        else
            ((struct VanargandPart *)obj)->sprite.flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r6");
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp) : "r"(preserveMask ? bit : temp));
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
                    endFlag = 8;
                    endFlag &= flags;
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
                s32 speed;
                speed = ScaleSequenceSpeed(state->frameWeight, state->speed);
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        }
    }
}

static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void Vector(struct SceneVec *v, s32 angle, s32 scale)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * scale);
    v->y = 0;
    {
        s32 sample = gUnk_086149C4[angle & 255];

        v->z = Scale(sample * scale);
    }
}
static inline u8 Ended(struct VanargandPart *p)
{
    return p->motion[0x29] == 2;
}

static inline void AddPlanar(struct SceneVec *v, struct SceneVec *origin)
{
    v->x = v->x + origin->x;
    v->z = v->z + origin->z;
}
// confidence=low: action role inferred from local motion and animation.
void Vanargand_UpdatePartSidestep(u8 *bossArg, struct VanargandPart *p)
{
    u32 mask = 12;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        {
            void *resource = *((void **)(bossArg + 0x4d0));
            Anim_SetSequence(&p->sequence, resource, 11, (u8)p->variant, 0);
        }
        StepSequence(p, 1, 0);
        {
            u32 zero = 0;
            p->state = zero;
            p->timer = zero;
        }
        p->enter = 1;
    }
    {
        u32 state = p->state;
        switch (state) {
            case 0: {
                struct SceneVec target;
                u32 entering = p->enter;
                u8 *enter;
                enter = &p->enter;
                if (entering) {
                    *enter = 0;
                    Vector(&target, *(((u8 *)p) + 0x187), 128);
                    AddPlanar(&target, &p->origin);
                    target.y = p->origin.y;
                    MotionTrack_Initialize(p->motion, &p->origin, &target, 0, 4);
                }
                MotionTrack_Update(p->motion);
                if (Ended(p)) {
                    u32 next = 1;
                    p->state = next;
                    p->timer = 0;
                    *enter = next;
                }
                break;
            }

            case 1:
                if (p->enter) {
                    p->enter = 0;
                    asm("" : "+r"(state));
                    {
                        void *resource = *((void **)(bossArg + 0x4d0));
                        Anim_SetSequence(&p->sequence, resource, 9, (u8)p->variant, 0);
                    }
                    StepSequence(p, state, 1);
                }
                {
                    register u32 finishedOffset asm("r2") = 0x170;
                    register u8 *finished asm("r1");
                    asm("" : "+r"(finishedOffset));
                    finished = (u8 *)p + finishedOffset;
                    *finished = 1;
                }
                break;
        }
    }
}
