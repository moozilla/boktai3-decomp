#include "contact_swarm.h"
// confidence=low: approaches screen X and periodically emits offset effects.
#include "scene_vec.h"
#include "ram.h"
#include "scene_departure_display.h"
// confidence=low: action role inferred from movement and effect calls.
extern const u16 gUnk_086141BC[];
s32 SceneMover_ApproachScreenX(struct DepartureActor *, s32);
s32 SceneMover_StepPastScreenX(struct DepartureActor *, s32, s32);
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
static inline void Update(struct DepartureActor *p, s32 time)
{
    ActorDisplay_UpdateScreenAnchor(p);
    ActorDisplay_UpdateForegroundSprite(p);
    ActorDisplay_UpdateAccentSprite(p);
    SceneActors_EmitPeriodicParticles(p, time);
}
static inline void SetHeldFlag(u8 *flags)
{
    u32 value = *flags;
    u32 mask = 4;
    mask |= value;
    *flags = mask;
}

void ActorEffect_SpawnOffsetEffect(struct DepartureActor *);
// confidence=low: sequence-one action periodically spawns offset effects.
void SceneActors_UpdatePeriodicEffectAction(struct DepartureActor *arg, s32 ticks)
{
    struct DepartureActor *input = arg;
    s32 time = ticks;
    struct DepartureActor *p = input;
    u32 one;
    {
        u32 random = RandomBits(0xFFFFFFFF);
        s32 *destination = (s32 *)((u8 *)input + 0x288);
        one = 1;
        *destination = random & one;
    }
    {
        u8 *entry = (u8 *)input + 0xb1;
        if (*entry) {
            struct DepartureDrawable *foreground;
            u32 zero = 0;
            *entry = zero;
            foreground = &input->foreground;
            W(0x298) = 0;
            W(0x290) = 0;
            foreground->sequenceId = one;
            {
                struct DisplaySequenceState *state = &foreground->sequence;
                Anim_SetSequence(state, PTR(0x390), 1, 0, 0);
                PlayerCue_StepSequenceEndedWithFirstFlipMask(&foreground->sprite, state, one);
            }
            W(0x598) = 0;
            W(0x398) = 0;
            W(0x2b8) = 4;
            W(0x2bc) = 120;
        }
    }
    SceneMover_ApproachScreenX(input, 40);
    switch (W(0x398)) {
        case 0:
            if (time > 30) {
                p->foreground.sequenceId = 1;
                Anim_SetSequence(&p->foreground.sequence, PTR(0x390), 1, 0, 0);
                W(0x398) = 1;
                if (W(0x2b4) <= 0x3fff) {
                    W(0x2b8) += 2;
                    W(0x2bc) = Div(W(0x2bc), 2);
                }
            }
            W(0x290) = RandomBits(7);
            Update(input, time);
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
            Update(input, time);
            if (Mod(time, W(0x2bc)) == 0 && time <= W(0x2bc) * W(0x2b8) + 30) {
                ActorEffect_SpawnOffsetEffect(input);
                W(0x598)++;
            }
            if (time >= W(0x2bc) * W(0x2b8) + 90) {
                W(0x304) = 0;
                p->foreground.sequenceId = 1;
                SetHeldFlag(&p->foreground.sequence.flags);
                Anim_RestartSequence(&p->foreground.sequence, PTR(0x390),
                                     (u16)p->foreground.sequenceId, 0, 0);
                W(0x398) = 2;
                W(0x290) = 0;
            }
            break;
        case 2:
            SetHeldFlag(&p->foreground.sequence.flags);
            Update(input, time);
            if (W(0x300)) {
                p->foreground.sequence.flags &= ~4;
                *((u8 *)input + 0xaf) = 1;
            }
            break;
    }
}
