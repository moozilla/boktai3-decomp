#include "contact_swarm.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: tentative player cue role; see callback body.
// Constant lifetimes, phase branches and particle configuration differ; particle words are fully initialized.
struct P {
    u8 pad3A2[0x3A2];
    u8 facing;
    u8 flip;
    u8 dir;
    u8 pad418[0x73];
    u8 variant;
    u8 pad42C[0x13];
    u16 f42C;
    u8 pad430[0x2];
    u16 f430;
    u8 f432;
    u8 f433;
    u8 pad458[0x24];
    u8 phase;
    u8 pad45A[0x1];
    u16 timer;
    u8 pad464[0x8];
    u8 f464;
    u8 f465;
    u8 pad4EE[0x88];
    u8 f4EE;
    u8 pad536[0x47];
    u16 f536;
    u16 f538;
    u8 pad5A3[0x69];
    u8 f5A3;
    u8 pad6FC[0x158];
    u16 f6FC;
    u8 padAC4[0x3C6];
    u16 fAC4;
    u8 padAD6[0x10];
    u16 fAD6;
    u16 fAD8;
};
void ActorSequence_Begin(struct P *, s32, s32);
void Player_AddControlFlags(struct P *, u32);
void PlayerCue_UpdateHidden(struct P *);
void PlayerCue_CompleteAction(struct P *);
void PlayerCue_EnterStandUpdate(struct P *);
void Player_RestartAnimationFromSetup(struct P *);
extern const s16 gUnk_086149C4[];
void Sound_PlaySong(u32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
#define U8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define U16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S16(p, o) (*(s16 *)((u8 *)(p) + (o)))
void Player_RestartAnimationFromResource(struct P *);
void Player_UpdateFacingSide(struct P *);
u32 Player_SetAnimation(struct P *, u32, u32, u32);
void SpiralTrail_CreateWithParticles(struct P *, s32, void *, s32, s32);
extern u8 *gUnk_030042E4;
s32 Player_ConsumeStateEntry(struct P *);
s32 ActorVariant_GetTableValue(s32, s32);
void Player_SetActionPhase(struct P *, s32);
void Player_InitVisualResources(struct P *);
// confidence=low: toggles actor variant between zero and one through four motion phases.
static inline u32 Arg1(u32 k_, struct P *p)
{
    return p->timer >> k_;
}

inline static u32 Arg1_1(struct P *p)
{
    return p->timer >> 2;
}

void PlayerCue_UpdateFormChange(struct P *p)
{
    if (Player_ConsumeStateEntry(p)) {
        p->fAD6 = 0x16F;
        p->fAD8 = 0;
        p->f4EE = 1;
        Sound_PlaySong(0x306);
        if (p->variant != 1) {
            p->f432 = 1;
            p->f433 = 1;
        } else {
            p->f432 = 0;
            p->f433 = 1;
        }
    }
    p->f430 = 4000;
    Player_AddControlFlags(p, 8);
    switch (p->phase) {
        case 0:
            if (((u8)(p->dir - 2)) <= 4) {
                p->dir = 4;
                p->facing = 4;
                p->flip = 0;
            } else {
                p->dir = 0;
                p->facing = 0;
                p->flip = 0;
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->variant, 0), 64, 1);
            p->facing >>= 2;
            if (p->f4EE) {
                struct {
                    u32 a;
                    u8 b;
                } particles;
                particles.a = 0;
                particles.b = 15;
                SpiralTrail_CreateWithParticles(p, 1, &particles, 50, 69);
                Sound_PlaySong(0x39B);
            }
            Player_SetActionPhase(p, 1);
            break;

        case 1:
            if (p->f4EE) {
                p->fAD8 += 3;
                if (p->fAD8 > 32)
                    p->fAD8 = 32;
            }
            p->f6FC = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->variant, 8), 64, 1)) {
                Player_SetActionPhase(p, 2);
                p->f42C = 0;
                p->f430 = 4000;
            }
            break;

        case 2:
            if ((p->timer & 7) < (7 - ((u16)Arg1_1(p))))
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->variant, 10), 64, 1);
            else if (p->variant != 1) {
                Anim_SetKeyframes(((u8 *)p) + 0x8C, ((u8 *)p) + 0x6C, 0xE4, 0, 0);
                Anim_Step(((u8 *)p) + 0x8C, ((u8 *)p) + 0x6C);
            } else
                Player_SetAnimation(p, ActorVariant_GetTableValue(0, 10), 64, 1);
            p->fAD8 += 3;
            if (p->fAD8 > 32)
                p->fAD8 = 32;
            p->f6FC = 30;
            if ((++p->timer) > 19) {
                if (p->variant != 1)
                    p->variant = 1;
                else
                    p->variant = 0;
                Player_InitVisualResources(p);
                p->fAC4 = 65535;
                if ((p->f536 != 21) && (p->f536 != 49))
                    p->f538 = 0;
                Player_SetActionPhase(p, 3);
            }
            break;

        case 3:
            p->f6FC = 15;
            if (p->variant != 1) {
                if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->variant, 11), 64, 1)) {
                    Player_UpdateFacingSide(p);
                    ActorSequence_Begin(p, 0, 0);
                    Player_UpdateFacingSide(p);
                    p->f464 = 1;
                    p->f465 = 0;
                    Player_UpdateFacingSide(p);
                    PlayerCue_EnterStandUpdate(p);
                    p->f5A3 = 1;
                }
            } else {
                if (p->timer == 0)
                    Anim_SetKeyframes(((u8 *)p) + 0x8C, ((u8 *)p) + 0x6C, 0xE4, 1, 0);
                Anim_Step(((u8 *)p) + 0x8C, ((u8 *)p) + 0x6C);
                if ((++p->timer) > 29) {
                    Player_UpdateFacingSide(p);
                    ActorSequence_Begin(p, 0, 0);
                    Player_UpdateFacingSide(p);
                    p->f464 = 1;
                    p->f465 = 1;
                    Player_UpdateFacingSide(p);
                    PlayerCue_EnterStandUpdate(p);
                    p->f5A3 = 1;
                }
            }
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", PlayerCue_UpdateFormChange);
#endif
