#include "contact_swarm.h"
#include "ram.h"
#include "player_cue_sequence.h"
// confidence=low: hit callback slows this departure action.
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
void Player_GetWorldPosition(s32, s32 *);
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
static inline void SetHeldFlag(u8 *flags)
{
    u32 value = *flags;
    u32 mask = 4;
    mask |= value;
    *flags = mask;
}

// confidence=low: phase meanings inferred from state entry and motion calls.
void SceneActors_UpdateHitSensitiveDeparture(struct P *arg, s32 ticks)
{
    struct P *input = arg;
    s32 time = ticks;
    struct P *p = input;
    {
        u8 *entry = (u8 *)input + 0xb1;
        if (*entry) {
            struct ContactSwarmSprite *sprite;
            u32 zero = 0;
            *entry = zero;
            sprite = &input->sprite;
            W(0x298) = 0;
            W(0x290) = 0;
            W(0x294) = 40;
            *(s32 *)((u8 *)sprite + 0x3c) = 2;
            {
                struct DisplaySequenceState *state = &input->seq;
                Anim_SetSequence(state, PTR(0x390), 2, 0, 0);
                PlayerCue_StepSequenceEnded(sprite, state);
            }
            W(0x398) = 0;
            Sound_PlaySong(0x586);
        }
    }
    switch (W(0x398)) {
        case 0:
            if (time > 90) {
                W(0x304) = 1;
                W(0x2fc) = 3;
                Anim_SetSequence(&p->seq, PTR(0x390), 3, 0, 0);
                PlayerCue_StepSequenceEnded(&p->sprite, &p->seq);
                W(0x398) = 1;
            }
            W(0x288) = RandomBits(1);
            SceneMover_ApproachScreenX(input, W(0x294));
            Update(input, time);
            break;

        case 1:
            if (time > 150) {
                W(0x304) = 0;
                W(0x398) = 2;
                W(0x294) = 160;
            }
            W(0x288) = RandomBits(3);
            SceneMover_ApproachScreenX(input, W(0x294));
            Update(input, time);
            break;

        case 2: {
            s32 velocity = Player_GetHorizontalVelocity(PTR(0x3a0));
            s32 position[4];
            if (velocity < 0x6000)
                velocity = 0x6000;
            if (S(0x5c2) > 0)
                velocity >>= 1;
            SceneMover_StepPastScreenX(input, W(0x294), velocity);
            W(0x288) = RandomBits(7);
            Update(input, time);
            if (W(0x300))
                W(0x304) = 1;
            Player_GetWorldPosition(0, position);
            if (S(0x278) > 159 || time > 420) {
                W(0x304) = 0;
                W(0x2fc) = 3;
                SetHeldFlag(&p->seq.flags);
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
            Update(input, time);
            W(0x304) = 0;
            W(0x2fc) = 3;
            SetHeldFlag(&p->seq.flags);
            Anim_RestartSequence(&p->seq, PTR(0x390), H(0x2fc), 0, 0);
            W(0x398) = 4;
            W(0x294) = 60;
            break;

        case 4:
            SceneMover_ApproachScreenX(input, W(0x294));
            SetHeldFlag(&p->seq.flags);
            W(0x288) = RandomBits(1);
            Update(input, time);
            if (W(0x300)) {
                p->seq.flags &= ~4;
                *((u8 *)input + 0xaf) = 1;
            }
            break;
    }
}
