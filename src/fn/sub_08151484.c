// confidence=low: tentative cue role inferred from named callers (PlayerCue_AimHold/AimEnd).
#include "global.h"
struct P {
    u8 pad0[0x418];
    u8 variant;
    u8 pad1[0x458 - 0x419];
    u8 phase;
    u8 pad2;
    u16 timer;
    u8 pad3[0x59B - 0x45C];
    u8 cuePhase;
    u8 pad4[0xAC9 - 0x59C];
    u8 flagAC9;
};
void Sound_PlaySong(u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct P *, s32, s32, s32);
void ActorSequence_Begin(struct P *, s32, s32);
void ActorGlyphTrail_Create(struct P *, s32, s32);
void PlayerCue_CompleteAction(struct P *);
void PlayerCue_EnterStandUpdate(struct P *);

#define PlayerCue_UpdateAim sub_08151484
void PlayerCue_UpdateAim(struct P *p)
{
    p->flagAC9 = 4;
    switch (p->phase) {
        case 0:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->variant, 0x26), 0x40, 1)) {
                Sound_PlaySong(0x1F7);
                ActorSequence_Begin(p, 3, 1);
                PlayerCue_CompleteAction(p);
            }
            break;
        case 1:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->variant, 0x27), 0x40, 1);
            p->timer++;
            if (p->timer > 5) {
                ActorSequence_Begin(p, 3, 2);
                ActorGlyphTrail_Create(p, 0x20, p->cuePhase);
            }
            break;
        case 2:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->variant, 0x27), 0x40, 1);
            if (p->timer == 6)
                Sound_PlaySong(0x277);
            p->timer++;
            if (p->timer > 0x3b) {
                Sound_PlaySong(0x278);
                ActorSequence_Begin(p, 3, 3);
            }
            break;
        case 3:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->variant, 0x27), 0x40, 1);
            p->timer++;
            if (p->timer > 7) {
                ActorSequence_Begin(p, 3, 4);
                PlayerCue_CompleteAction(p);
            }
            break;
        case 4:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->variant, 0x27), 0x40, 1);
            break;
        case 5:
            if (p->timer == 0) {
                Sound_PlaySong(0x1F6);
                p->timer++;
            }
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->variant, 0x28), 0x40, 1))
                ActorSequence_Begin(p, 3, 6);
            break;
        case 6:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->variant, 0x27), 0x40, 1);
            p->timer++;
            if (p->timer > 0xe)
                ActorSequence_Begin(p, 3, 7);
            break;
        case 7:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->variant, 0x26), 0x40, 1)) {
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
