// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: emits six radial effects, waits for animation 10, then deactivates the part.

// confidence=low: inferred from adjacent Vanargand part callbacks and local state transitions.

// Remaining differences: radial-loop allocation and animation-step register lifetimes.
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
extern const s16 gUnk_086149C4[];
void CpuSet(const void *, void *, u32);
void Sound_PlaySong(u32);
void VanargandEffect_Create(u32, s32, const void *, const void *, u32);
void ActorStatus_Deactivate(void *, void *);
static inline s32 TruncateScale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void MakeVector(struct SceneVec *out, s32 angle, s32 scale)
{
    out->x = TruncateScale(gUnk_086149C4[(angle + 64) & 255] * scale);
    out->y = 0;
    out->z = TruncateScale(gUnk_086149C4[angle & 255] * scale);
}
static inline void AddVector(struct SceneVec *dst, struct SceneVec *a, struct SceneVec *b)
{
    dst->x = b->x + a->x;
    dst->y = b->y + a->y;
    dst->z = b->z + a->z;
}
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
        register u32 temp asm("r2");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        mask = temp;
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                s16 index = state->index;
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

static inline void SetState(struct VanargandPart *p, u32 state)
{
    p->state = state;
    p->timer = 0;
}
void Vanargand_ReleaseRadialEffects(u8 *boss, struct VanargandPart *p)
{
    u32 mask = 12;
    u32 *new_var;
    p->flags |= mask;
    if (p->reset) {
        u32 zero;
        struct SceneVec velocity;
        struct SceneVec position;
        s32 i;
        s32 phase;
        u32 *state;
        u32 *timer;
        u8 *entered;
        p->reset = 0;
        Anim_SetSequence(&p->sequence, *((void **)(boss + 0x4d0)), 10, (u8)p->variant, 0);
        CpuSet(&zero, &velocity, 0x05000002);
        new_var = &p->timer;
        zero = 0;
        i = 0;
        state = &p->state;
        timer = new_var;
        entered = &p->enter;
        phase = 0;
        do {
            MakeVector(&position, phase / 6, 128);
            AddVector(&position, (struct SceneVec *)(&p->sprite.position), &position);
            VanargandEffect_Create(1, i << 1, &position, &velocity, p->variant);
            phase += 255;
            i++;
        } while (i <= 5);
        Sound_PlaySong(0x58c);
        *state = 0;
        *timer = 0;
        *entered = 1;
    }
    switch (p->state) {
        case 0:
            if (StepPlayback(p, &p->sequence, p, 1)) {
                SetState(p, 1);
                p->enter = 1;
            }
            break;

        case 1:
            p->finished = 1;
            ActorStatus_Deactivate(boss, p);
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Vanargand_ReleaseRadialEffects);
#endif
