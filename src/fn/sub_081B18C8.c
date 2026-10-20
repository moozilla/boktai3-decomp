// MATCHING_CONCESSION: register-pinning
#include "global.h"

// confidence=low: rises, waits for a countdown, then returns vertically
#ifdef NONMATCHING
// fuzzy: 80.3%
// Remaining differences: State-address reuse, entry-pointer lifetimes, and vector/constant ordering differ.
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
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);
void Sound_PlaySong(u32);
static inline u32 StepPlayback(struct ActorStatusPart *owner, struct DisplaySequenceState *sequence,
                               struct ActorStatusPart *base, u32 one)
{
    register struct DisplaySequenceState *state asm("r4") = sequence;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = base->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = one;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
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
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->spriteFlags |= 8;
        else
            owner->spriteFlags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r5");
        register u32 temp asm("r3");
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

static inline void SetAnimation(struct ActorStatusPart *p, u8 *boss, u32 id, u32 options)
{
    void *data = *(void **)(boss + 0x4d0);
    register struct DisplaySequenceState *seq asm("r4") = &p->sequence;
    Anim_SetSequence(seq, data, id, (u8)p->variant, options);
    StepPlayback(p, seq, p, 1);
}
static inline void SetVector(struct SceneVec *p, s16 x, s16 y, s16 z)
{
    ((volatile s16 *)p)[0] = x;
    p->y = y;
    p->z = z;
}
static inline void AddOffset(struct SceneVec *out, struct SceneVec *a, struct SceneVec *b)
{
    out->x = (u16)b->x + (u16)a->x;
    out->y = (u16)a->y + (u16)b->y;
    out->z = (u16)b->z + (u16)a->z;
}
static inline u8 MotionEnded(struct ActorStatusPart *p)
{
    u8 result = 0;
    if (p->motion[0x29] == 2)
        result = 1;
    return result;
}
void ActorStatus_UpdateRisingStrike(u8 *boss, struct ActorStatusPart *p)
{
    struct SceneVec rise, target, speed;
    u32 mask = 0x20;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        SetAnimation(p, boss, 8, 4);
        p->state = 0;
        p->timer = 0;
        p->enter = 1;
    }
    {
        register u32 currentState asm("r4") = p->state;
        asm("" : "+r"(currentState));
        switch (currentState) {
            case 0: {
                u8 *enter = &p->enter;
                register void *motion asm("r8") = p->motion;
                if (*enter) {
                    *enter = 0;
                    SetVector(&rise, 0, 512, 0);
                    {
                        struct SceneVec *origin = &p->offset;
                        AddOffset(&rise, origin, &rise);
                        MotionTrack_Initialize(motion, origin, &rise, 0, 4);
                    }
                }
                MotionTrack_Update(motion);
                if (MotionEnded(p)) {
                    p->state = 1;
                    p->timer = 0;
                    *enter = 1;
                }
                break;
            }
            case 1: {
                u8 *enter = &p->enter;
                if (*enter) {
                    *enter = 0;
                    Sound_PlaySong(0x596);
                    p->countdown = 60;
                }
                if (!p->countdown) {
                    p->state = 2;
                    p->timer = 0;
                    *enter = 1;
                }
                break;
            }
            case 2: {
                u8 *enter = &p->enter;
                register void *motion asm("r8") = p->motion;
                if (*enter) {
                    *enter = 0;
                    target = p->offset;
                    ((volatile s16 *)&target)[1] = -(u16)p->anchor->y;
                    SetVector(&speed, 0, 64, 0);
                    MotionTrack_Initialize(motion, &p->offset, &target, &speed, 1);
                }
                MotionTrack_Update(motion);
                if (MotionEnded(p)) {
                    p->state = 3;
                    p->timer = 0;
                    *enter = 1;
                }
                break;
            }
            case 3:
                p->finished = 1;
                break;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", ActorStatus_UpdateRisingStrike);
#endif
