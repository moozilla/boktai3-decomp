// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 91.3%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: action role inferred from local animation, motion and effects.
#include "contact_swarm.h"
#include "scene_vec.h"
// Remaining differences: register lifetimes, instruction order and literal pools.

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
static inline u32 StepPlayback(struct VanargandPart *owner, struct DisplaySequenceState *state,
                               struct VanargandPart *base, u32 one)
{
    struct ContactSwarmSprite *sprite = &owner->sprite;
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = base->sequence.commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = one;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
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
        if (first != ((((*command) & 0x30) >> 4) & bit))
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u16 temp asm("r2");
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

// confidence=low: paired part travels to an impact point and emits a burst.
void MotionTrack_Update(void *);
s32 ScenePair_CountPositiveCounters(void *);
void Sound_PlaySong(u32);
void AnimatedParticles_EmitBurst(void *, s32, s32, s32, s32, s32, s32, s32);
void Vanargand_EmitPartTrail(void *);
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void Set(struct SceneVec *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
static inline void Diff(struct SceneVec *v, struct SceneVec *a, struct SceneVec *b)
{
    v->x = a->x - b->x;
    v->y = a->y - b->y;
    v->z = a->z - b->z;
}
static inline void Vector(struct SceneVec *v, s32 angle, s32 scale)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * scale);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * scale);
}
static inline void Add(struct SceneVec *v, struct SceneVec *a, struct SceneVec *b)
{
    v->x = b->x + a->x;
    v->y = b->y + a->y;
    v->z = a->z + b->z;
}
static inline u8 Ended(struct VanargandPart *p)
{
    return p->motion[0x29] == 2;
}
static inline void SetState(struct VanargandPart *p, u32 state)
{
    p->state = state;
    p->timer = 0;
}
void Vanargand_UpdatePartImpact(u8 *boss, struct VanargandPart *p, u32 frame)
{
    u8 *enter;
    register u32 entering asm("r1");
    register u32 savedFrame asm("r8") = frame;
    u32 mask = 0x20;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        p->state = 0;
        p->timer = 0;
        p->enter = 1;
    }
    switch (p->state) {
        case 0:
            entering = p->enter;
            asm("" : "+r"(entering));
            enter = &p->enter;
            if (entering) {
                {
                    u32 zero = 0;
                    *enter = zero;
                    asm("" : "+r"(zero));
                    Anim_SetSequence(&p->sequence, *((void **)(boss + 0x4d0)), 8, (u8)p->variant,
                                     zero);
                }
            }
            if (StepPlayback(p, &p->sequence, p, 1)) {
                SetState(p, 1);
                *enter = 1;
            }
            break;

        case 1: {
            struct SceneVec delta;
            entering = p->enter;
            asm("" : "+r"(entering));
            enter = &p->enter;
            if (entering) {
                *enter = 0;
                if (p->variant == 1)
                    Vector(&delta, 224, 64);
                else
                    Vector(&delta, 96, 64);
                Add(&delta, &p->origin, &delta);
                MotionTrack_Initialize(p->motion, &p->origin, &delta, 0, 4);
            }
            MotionTrack_Update(p->motion);
            if (Ended(p)) {
                SetState(p, 2);
                *enter = 1;
            }
            break;
        }

        case 2:
            entering = p->enter;
            asm("" : "+r"(entering));
            enter = &p->enter;
            if (entering) {
                *enter = 0;
                if (p->variant == 1) {
                    Set(&p->target, 0x3ab, 0x100, 0x2cb);
                } else {
                    Set(&p->target, 0x2cb, 0x100, 0x3ab);
                }
                Diff(&p->target, &p->target, p->basePosition);
                MotionTrack_Initialize(p->motion, &p->origin, &p->target, 0, 2);
            }
            MotionTrack_Update(p->motion);
            if (Ended(p)) {
                if ((ScenePair_CountPositiveCounters(boss) == 2) &&
                    (boss[(p->variant == 1) ? (0x205) : (0x421)] == 12)) {
                    struct SceneVec point;
                    Sound_PlaySong(0x1ba);
                    Add(&point, p->basePosition, &p->target);
                    AnimatedParticles_EmitBurst(&point, p->particleAngle, 0, 64, 4, 4, 4, 4);
                    p->flashTimer = 15;
                    SetState(p, 3);
                } else {
                    Sound_PlaySong(0x3d9);
                    AnimatedParticles_EmitBurst(((u8 *)p) + 0x1c, p->particleAngle, 0, 64, 4, 4, 4,
                                                4);
                    SetState(p, 3);
                }
                *enter = 1;
                p->finished = 1;
            } else if (!(savedFrame & 1))
                Vanargand_EmitPartTrail(p);
            break;

        case 3:
            p->finished = 1;
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Vanargand_UpdatePartImpact);
#endif
