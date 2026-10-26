// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
// confidence=low: approaches screen X and periodically emits offset effects.
#include "scene_vec.h"
#include "ram.h"
// confidence=low: action role inferred from movement and effect calls.
// Remaining differences: the +0x598 reset offset uses r3 instead of r1.
struct P {
    u8 pad0[0x2c0];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState seq;
};
extern const u16 gUnk_086141BC[];
void ActorDisplay_UpdateScreenAnchor(struct P *);
void ActorDisplay_UpdateForegroundSprite(struct P *);
void ActorDisplay_UpdateAccentSprite(struct P *);
void SceneActors_EmitPeriodicParticles(struct P *, s32);
s32 SceneMover_ApproachScreenX(struct P *, s32);
s32 SceneMover_StepPastScreenX(struct P *, s32, s32);
s32 Player_GetHorizontalVelocity(void *);
void Player_GetWorldPosition(s32, struct SceneVec *);
void Anim_RestartSequence(struct DisplaySequenceState *, const void *, u16, s32, s32);
#define B(o)   (*(u8 *)((u8 *)p + (o)))
#define H(o)   (*(u16 *)((u8 *)p + (o)))
#define S(o)   (*(s16 *)((u8 *)p + (o)))
#define W(o)   (*(s32 *)((u8 *)p + (o)))
#define PTR(o) (*(void **)((u8 *)p + (o)))
static inline u32 RandomValue(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    return gUnk_086141BC[gEffectRandomTableIndex];
}
#define RandomBits(mask) (RandomValue() & (mask))
static inline void Update(struct P *p, s32 time)
{
    ActorDisplay_UpdateScreenAnchor(p);
    ActorDisplay_UpdateForegroundSprite(p);
    ActorDisplay_UpdateAccentSprite(p);
    SceneActors_EmitPeriodicParticles(p, time);
}
static inline void Step(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st, u32 bitOne)
{
    struct ContactSwarmSprite *sprite = sp;
    struct DisplaySequenceState *state = st;
    const volatile u16 *command;
    u32 duration, flags;
    u32 bit;
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
        u32 maskBit = bitOne;
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
        u32 mask;
        register u32 temp asm("r3");
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

void ActorEffect_SpawnOffsetEffect(struct P *);
// confidence=low: sequence-one action periodically spawns offset effects.
void SceneActors_UpdatePeriodicEffectAction(struct P *arg, s32 ticks)
{
    struct P *input = arg;
    s32 time = ticks;
    struct P *p = input;
    u32 one;
    {
        u32 random = RandomBits(0xFFFFFFFF);
        s32 *destination = (s32 *)((u8 *)input + 0x288);
        register u32 mask asm("r2") = 1;
        one = mask;
        *destination = random & mask;
    }
    {
        u8 *entry = (u8 *)input + 0xb1;
        if (*entry) {
            struct ContactSwarmSprite *sprite;
            u32 zero = 0;
            *entry = zero;
            sprite = &input->sprite;
            (*(s32 *)((u8 *)input + 0x298)) = 0;
            (*(s32 *)((u8 *)input + 0x290)) = 0;
            {
                register u32 sequence asm("r3") = one;
                asm("" : "+r"(sequence));
                *(s32 *)((u8 *)sprite + 0x3c) = sequence;
            }
            {
                struct DisplaySequenceState *state = &input->seq;
                Anim_SetSequence(state, (*(void **)((u8 *)input + 0x390)), 1, 0, 0);
                Step(sprite, state, one);
            }
            {
                register u32 off asm("r1") = 0x598;
                register u8 *q asm("r0");
                asm("" : "+r"(off));
                q = (u8 *)p + off;
                *(s32 *)q = 0;
            }
            {
                register u32 off asm("r2") = 0x398;
                register u8 *q asm("r0");
                s32 z = 0;
                asm("" : "+r"(off));
                q = (u8 *)p + off;
                *(s32 *)q = z;
            }
            {
                register u32 off asm("r3") = 0x2b8;
                register u8 *q asm("r1");
                asm("" : "+r"(off));
                q = (u8 *)p + off;
                *(s32 *)q = 4;
            }
            {
                register u32 off asm("r0") = 0x2bc;
                register u8 *q asm("r1");
                asm("" : "+r"(off));
                q = (u8 *)p + off;
                *(s32 *)q = 120;
            }
        }
    }
    SceneMover_ApproachScreenX(input, 40);
    {
        register u32 poff asm("r1") = 0x398;
        s32 *phase;
        asm("" : "+r"(poff));
        phase = (s32 *)((u8 *)p + poff);
        switch (*phase) {
            case 0:
                if (time > 30) {
                    W(0x2fc) = 1;
                    Anim_SetSequence(&p->seq, PTR(0x390), 1, 0, 0);
                    *phase = 1;
                    if (W(0x2b4) <= 0x3fff) {
                        W(0x2b8) += 2;
                        W(0x2bc) = Div(W(0x2bc), 2);
                    }
                }
                W(0x290) = RandomBits(7);
                ActorDisplay_UpdateScreenAnchor(input);
                ActorDisplay_UpdateForegroundSprite(input);
                ActorDisplay_UpdateAccentSprite(input);
                SceneActors_EmitPeriodicParticles(input, time);
                if (W(0x300))
                    W(0x304) = 1;
                break;
            case 1:
                if (Mod(time, W(0x2bc)) <= 59 && time > 90)
                    W(0x290) = 0;
                else if (time < W(0x2bc) * W(0x2b8) + 30)
                    W(0x290) = RandomBits(7);
                else
                    W(0x290) = 0;
                ActorDisplay_UpdateScreenAnchor(input);
                ActorDisplay_UpdateForegroundSprite(input);
                ActorDisplay_UpdateAccentSprite(input);
                SceneActors_EmitPeriodicParticles(input, time);
                if (Mod(time, W(0x2bc)) == 0 && time <= W(0x2bc) * W(0x2b8) + 30) {
                    ActorEffect_SpawnOffsetEffect(input);
                    W(0x598)++;
                }
                if (time >= W(0x2bc) * W(0x2b8) + 90) {
                    W(0x304) = 0;
                    W(0x2fc) = 1;
                    {
                        u32 flags = B(0x2f0);
                        u32 mask = 4;
                        asm("" : "+r"(flags), "+r"(mask));
                        B(0x2f0) = mask | flags;
                    }
                    Anim_RestartSequence(&p->seq, PTR(0x390), H(0x2fc), 0, 0);
                    W(0x398) = 2;
                    W(0x290) = 0;
                }
                break;
            case 2: {
                u32 flags = B(0x2f0);
                u32 mask = 4;
                asm("" : "+r"(flags), "+r"(mask));
                B(0x2f0) = mask | flags;
            }
                ActorDisplay_UpdateScreenAnchor(input);
                ActorDisplay_UpdateForegroundSprite(input);
                ActorDisplay_UpdateAccentSprite(input);
                SceneActors_EmitPeriodicParticles(input, time);
                if (W(0x300)) {
                    B(0x2f0) &= ~4;
                    *((u8 *)input + 0xaf) = 1;
                }
                break;
        }
    }
}
