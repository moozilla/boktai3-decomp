// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 96.1%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: provisional role inferred from state transitions and player-motion calls

// confidence=low: rises, waits for a countdown, then returns vertically
// Remaining differences: Entry-pointer lifetimes, derived vector constants, and state-transition allocation differ.
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
    struct DisplaySequenceState *state = sequence;
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
        u32 maskBit = one;
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
        u32 mask;
        u32 temp;
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
    void *data = *((void **)(boss + 0x4d0));
    struct DisplaySequenceState *seq;
    register u32 playbackOptions asm("r7");
    typeof(playbackOptions) c1;
    playbackOptions = options;
    c1 = playbackOptions;
    seq = &p->sequence;
    Anim_SetSequence(seq, data, id, (u8)p->variant, playbackOptions);
    StepPlayback(p, seq, p, 1);
}

static inline void SetVector(struct SceneVec *p, s32 x, s32 y, s32 z)
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
    struct SceneVec *new_var;
    struct SceneVec rise;
    struct SceneVec target;
    struct SceneVec speed;
    u8 *enter;
    u32 mask = 0x20;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        SetAnimation(p, boss, 8, 4);
        {
            u32 zero = 0;
            p->state = zero;
            p->timer = zero;
        }
        p->enter = 1;
    }
    {
        register u32 offset asm("r1") = 0x164;
        u32 currentState;
        {
            u32 *stateAddress = (u32 *)(((u8 *)p) + offset);
            currentState = *stateAddress;
        }
        switch (currentState) {
            case 0: {
                u32 entered;
                register void *motion asm("r8");
                {
                    register u8 *address asm("r0") = &p->enter;
                    entered = *address;
                    enter = address;
                }
                motion = p->motion;
                if (entered) {
                    u32 zero = 0;
                    *enter = zero;
                    SetVector(&rise, zero, 512, zero);
                    {
                        struct SceneVec *origin = &p->offset;
                        AddOffset(&rise, origin, &rise);
                        MotionTrack_Initialize(motion, origin, &rise, 0, 4);
                    }
                }
                MotionTrack_Update(motion);
                if (MotionEnded(p)) {
                    u32 next = 1;
                    p->state = next;
                    p->timer = 0;
                    *enter = next;
                }
                break;
            }

            case 1: {
                u32 entered;
                {
                    register u8 *address asm("r0") = &p->enter;
                    entered = *address;
                    enter = address;
                }
                if (entered) {
                    {
                        u8 k2 = 0;
                        *enter = k2;
                    }
                    Sound_PlaySong(0x596);
                    p->countdown = 60;
                }
                if (!p->countdown) {
                    {
                        u8 k1 = 2;
                        p->state = k1;
                    }
                    p->timer = 0;
                    *enter = currentState;
                }
                break;
            }

            case 2: {
                u32 entered;
                void *motion;
                {
                    register u8 *address asm("r0") = &p->enter;
                    entered = *address;
                    enter = address;
                    asm("" : "+r"(enter));
                }
                motion = p->motion;
                if (entered) {
                    register u32 zero asm("r5") = 0;
                    asm("" : "+r"(zero));
                    new_var = &target;
                    *enter = zero;
                    target = p->offset;
                    ((volatile s16 *)new_var)[1] = -((u16)p->anchor->y);
                    SetVector(&speed, zero, 64, zero);
                    MotionTrack_Initialize(motion, &p->offset, new_var, &speed, 1);
                }
                MotionTrack_Update(motion);
                if (MotionEnded(p)) {
                    {
                        register u32 next asm("r1") = 3;
                        asm("" : "+r"(next));
                        p->state = next;
                    }
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
