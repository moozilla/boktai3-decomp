// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "scene_vec.h"
#ifdef NONMATCHING
// fuzzy: 82.2%
// Remaining differences: random-mask ordering and allocation in the second animation step.
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

#define SceneActors_UpdateSequenceTwoCharge                                                        \
    sub_081EE518 // confidence=low: sequence two leads into horizontal charge and recovery.
void SceneActors_UpdateSequenceTwoCharge(struct P *arg, s32 ticks)
{
    register struct P *input asm("r8") = arg;
    register s32 time asm("r9") = ticks;
    register struct P *p asm("r7") = input;
    if ((*(u8 *)((u8 *)input + 0xb1))) {
        struct ContactSwarmSprite *sprite;
        (*(u8 *)((u8 *)input + 0xb1)) = 0;
        sprite = &input->sprite;
        (*(s32 *)((u8 *)input + 0x298)) = 0;
        (*(s32 *)((u8 *)input + 0x290)) = 0;
        (*(s32 *)((u8 *)input + 0x294)) = 40;
        (*(s32 *)((u8 *)sprite + 0x3c)) = 2;
        {
            register struct DisplaySequenceState *state asm("r4") = &input->seq;
            Anim_SetSequence(state, (*(void **)((u8 *)input + 0x390)), 2, 0, 0);
            Step(sprite, state);
        }
        W(0x398) = 0;
        Sound_PlaySong(0x586);
    }
    switch ((u32)W(0x398)) {
        case 0:
            if (time > 90) {
                W(0x304) = 1;
                W(0x2fc) = 3;
                {
                    register struct DisplaySequenceState *state asm("r4") = &p->seq;
                    Anim_SetSequence(state, PTR(0x390), 3, 0, 0);
                    Step(&p->sprite, state);
                }
                W(0x398) = 1;
            }
            W(0x288) = RandomBits(1);
            goto approach;
        case 1:
            if (time > 150) {
                W(0x304) = 0;
                W(0x398) = 2;
                W(0x294) = 160;
            }
            W(0x288) = RandomBits(3);
        approach:
            SceneMover_ApproachScreenX(input, W(0x294));
            sub_081EC6BC(input);
            sub_081EC7D8(input);
            sub_081EC974(input);
            SceneActors_EmitPeriodicParticles(input, time);
            break;
        case 2: {
            s32 velocity = Player_GetHorizontalVelocity(PTR(0x3a0));
            s32 position[4];
            if (velocity <= 0x5fff)
                velocity = 0x6000;
            if (S(0x5c2) > 0)
                velocity >>= 1;
            sub_081ECE80(input, W(0x294), velocity);
            W(0x288) = RandomBits(7);
            sub_081EC6BC(input);
            sub_081EC7D8(input);
            sub_081EC974(input);
            SceneActors_EmitPeriodicParticles(input, time);
            if (W(0x300))
                W(0x304) = 1;
            Player_GetWorldPosition(0, (struct SceneVec *)position);
            if (S(0x278) > 159 || time > 420) {
                W(0x304) = 0;
                W(0x2fc) = 3;
                B(0x2f0) |= 4;
                Anim_RestartSequence(&p->seq, PTR(0x390), H(0x2fc), 0, 0);
                W(0x294) = 60;
                W(0x398) = 4;
            } else if (W(0x590) == 1) {
                W(0x294) = 60;
                W(0x398) = 3;
            }
            break;
        }
        case 3:
            SceneMover_ApproachScreenX(input, W(0x294));
            W(0x288) = RandomBits(1);
            sub_081EC6BC(input);
            sub_081EC7D8(input);
            sub_081EC974(input);
            SceneActors_EmitPeriodicParticles(input, time);
            W(0x304) = 0;
            W(0x2fc) = 3;
            B(0x2f0) |= 4;
            Anim_RestartSequence(&p->seq, PTR(0x390), H(0x2fc), 0, 0);
            W(0x398) = 4;
            W(0x294) = 60;
            break;
        case 4:
            SceneMover_ApproachScreenX(input, W(0x294));
            B(0x2f0) |= 4;
            W(0x288) = RandomBits(1);
            sub_081EC6BC(input);
            sub_081EC7D8(input);
            sub_081EC974(input);
            SceneActors_EmitPeriodicParticles(input, time);
            if (W(0x300)) {
                B(0x2f0) &= ~4;
                B(0xaf) = 1;
            }
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081EE518);
#endif
