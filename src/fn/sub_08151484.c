// confidence=low: tentative cue role inferred from named callers (PlayerCue_AimHold/AimEnd).
#include "player_cue.h"
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void ActorGlyphTrail_Create(struct Player *, s32, s32);
void PlayerCue_CompleteAction(struct Player *);
void PlayerCue_EnterStandUpdate(struct Player *);

void PlayerCue_UpdateAim(struct Player *p)
{
    p->actionVariant = 4;
    switch (p->tetherPhase) {
        case 0:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x26), 0x40, 1)) {
                Sound_PlaySong(0x1F7);
                ActorSequence_Begin(p, 3, 1);
                PlayerCue_CompleteAction(p);
            }
            break;
        case 1:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x27), 0x40, 1);
            p->tetherFrame++;
            if (p->tetherFrame > 5) {
                ActorSequence_Begin(p, 3, 2);
                ActorGlyphTrail_Create(p, 0x20, p->cuePhase);
            }
            break;
        case 2:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x27), 0x40, 1);
            if (p->tetherFrame == 6)
                Sound_PlaySong(0x277);
            p->tetherFrame++;
            if (p->tetherFrame > 0x3b) {
                Sound_PlaySong(0x278);
                ActorSequence_Begin(p, 3, 3);
            }
            break;
        case 3:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x27), 0x40, 1);
            p->tetherFrame++;
            if (p->tetherFrame > 7) {
                ActorSequence_Begin(p, 3, 4);
                PlayerCue_CompleteAction(p);
            }
            break;
        case 4:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x27), 0x40, 1);
            break;
        case 5:
            if (p->tetherFrame == 0) {
                Sound_PlaySong(0x1F6);
                p->tetherFrame++;
            }
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x28), 0x40, 1))
                ActorSequence_Begin(p, 3, 6);
            break;
        case 6:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x27), 0x40, 1);
            p->tetherFrame++;
            if (p->tetherFrame > 0xe)
                ActorSequence_Begin(p, 3, 7);
            break;
        case 7:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x26), 0x40, 1)) {
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
