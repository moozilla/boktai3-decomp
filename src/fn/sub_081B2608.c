// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"
// confidence=low: moves a Vanargand part along its heading, then plays sequence 7.
// confidence=low: provisional role inferred from adjacent Vanargand part callbacks.

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: state-transition address reuse, entry-pointer copies, and allocation.
struct VanargandPart {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x1c];
    struct DisplaySequenceState sequence;
    u8 motion[0x44];
    struct SceneVec origin;
    u8 unknownA4[8];
    s16 heading;
    u8 unknownAE[0xae];
    u32 variant;
    u32 flags;
    u32 state;
    u32 timer;
    u8 unknown16C[2];
    u8 reset, enter, finished;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);
extern const s16 gUnk_086149C4[];
static inline s32 TruncateScale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void RetreatVector(struct SceneVec *out, s32 angle)
{
    s32 scale = 720;
    s32 x = gUnk_086149C4[(angle + 64) & 255] * scale;
    out->x = TruncateScale(x);
    out->y = 0;
    x = gUnk_086149C4[angle & 255] * scale;
    out->z = TruncateScale(x);
}
static inline u32 StepPlayback(struct VanargandPart *owner, struct DisplaySequenceState *state,
                               struct VanargandPart *base, u32 one)
{
    struct ContactSwarmSprite *sprite = &owner->sprite;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = base->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = one;
        bit = maskBit;
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
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
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
        asm("" : "+r"(mask));
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
static inline void SetState(struct VanargandPart *p, u32 state)
{
    p->state = state;
    p->timer = 0;
}
static inline void SetFlags(struct VanargandPart *p, u32 mask)
{
    p->flags |= mask;
}
static inline u32 StepPlayback_UpdatePartRetreat(struct VanargandPart *owner,
                                                 struct DisplaySequenceState *state,
                                                 struct VanargandPart *base, u32 one)
{
    struct ContactSwarmSprite *sprite = &owner->sprite;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = base->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = one;
        bit = maskBit;
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
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
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
        asm("" : "+r"(mask));
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

void Vanargand_UpdatePartRetreat(u8 *boss, struct VanargandPart *p)
{
    register u8 *savedBoss asm("r9") = boss;
    u8 *enter;
    u32 state;
    struct SceneVec delta;
    u32 mask = 0x20;
    asm("" : "+r"(savedBoss));
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        if (p->variant == 0)
            RetreatVector(&delta, p->heading);
        else
            RetreatVector(&delta, p->heading);
        MotionTrack_Initialize(p->motion, &p->origin, &delta, 0, 4);
        {
            register u32 zero asm("r1") = 0;
            u32 offset = 0x164;
            asm("" : "+r"(zero));
            *((u32 *)(((u8 *)p) + offset)) = zero;
            p->timer = zero;
            *((u8 *)(((u8 *)p) - (-offset))) = 1;
            offset += 11;
        }
    }
    state = p->state;
    switch (state) {
        case 0:
            SetFlags(p, 8);
            MotionTrack_Update(p->motion);
            {
                u32 finished = p->motion[0x29] == 2;
                asm("" : "+r"(finished));
                if (finished) {
                    SetState(p, 1);
                    p->enter = 1;
                }
            }
            break;

        case 1:
            enter = &p->enter;
            if (*enter) {
                u32 value = *enter;
                asm("" : "+r"(value));
                *enter = (value = 0);
                Anim_SetSequence(&p->sequence, *((void **)(savedBoss + 0x4d0)), 7, (u8)p->variant,
                                 0);
            }
            if (StepPlayback_UpdatePartRetreat(p, &p->sequence, p, state)) {
                SetState(p, 2);
                *enter = 1;
            }
            break;

        case 2:
            p->finished = 1;
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Vanargand_UpdatePartRetreat);
#endif
