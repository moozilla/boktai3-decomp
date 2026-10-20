// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "scene_vec.h"
#ifdef NONMATCHING
// fuzzy: 91.6%
// Remaining differences: entry/reset allocation and the second sprite stepper.
struct P {
    u8 pad0[0x2c0];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState seq;
};
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
void sub_081EC6BC(struct P *);
void sub_081EC7D8(struct P *);
void sub_081EC974(struct P *);
void SceneActors_EmitPeriodicParticles(struct P *, s32);
s32 SceneMover_ApproachScreenX(struct P *, s32);
s32 sub_081ECE80(struct P *, s32, s32);
s32 Player_GetHorizontalVelocity(void *);
void Player_GetWorldPosition(s32, struct SceneVec *);
void Anim_RestartSequence(struct DisplaySequenceState *, const void *, u16, s32, s32);
#define B(o)   (*(u8 *)((u8 *)p + (o)))
#define H(o)   (*(u16 *)((u8 *)p + (o)))
#define S(o)   (*(s16 *)((u8 *)p + (o)))
#define W(o)   (*(s32 *)((u8 *)p + (o)))
#define PTR(o) (*(void **)((u8 *)p + (o)))
static inline u32 RandomBits(u32 mask)
{
    gUnk_03005304 = (gUnk_03005304 + 1) & 1023;
    return gUnk_086141BC[gUnk_03005304] & mask;
}
static inline void Update(struct P *p, s32 time)
{
    sub_081EC6BC(p);
    sub_081EC7D8(p);
    sub_081EC974(p);
    SceneActors_EmitPeriodicParticles(p, time);
}
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
        asm volatile("" : "+r"(bit));
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
        asm volatile("" : "+r"(bit));
        first = bit;
        first &= input;
        asm volatile("" : "+r"(first));
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
        asm volatile("" : "+r"(temp));
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
                    asm volatile("" : "+r"(endFlag));
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

#define SceneActors_UpdateTimedApproach                                                            \
    sub_081ED8A0 // confidence=low: two sequences precede a timed screen-X approach.
void SceneActors_UpdateTimedApproach(struct P *arg, s32 ticks)
{
    volatile s32 time = ticks;
    register struct P *input asm("r9") = arg;
    register s32 done asm("r10") = 0;
    register struct P *p asm("r8") = input;
    if (B(0xb1)) {
        struct ContactSwarmSprite *sprite;
        struct ContactSwarmSprite *second;
        register struct DisplaySequenceState *state asm("r4");
        B(0xb1) = done;
        sprite = &p->sprite;
        second = (void *)((u8 *)p + 0x308);
        W(0x298) = done;
        W(0x290) = done;
        *(s32 *)((u8 *)sprite + 0x3c) = done;
        state = &p->seq;
        Anim_SetSequence(state, PTR(0x390), 0, 0, done);
        Step(sprite, state);
        *(s32 *)((u8 *)second + 0x3c) = 4;
        state = (void *)((u8 *)second + 0x2c);
        Anim_SetSequence(state, PTR(0x390), 4, 0, 0);
        Step(second, state);
    }
    if (time > 120)
        done = sub_081ECE80(input, W(0x294), 0x6000);
    W(0x288) = RandomBits(1);
    sub_081EC6BC(input);
    sub_081EC7D8(input);
    sub_081EC974(input);
    SceneActors_EmitPeriodicParticles(input, time);
    if (done) {
        *((u8 *)input + 0xaf) = 1;
        W(0x284) = 16;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081ED8A0);
#endif
