#include "player_cue.h"

// confidence=low: tentative player cue field interpretations.
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
void PlayerCue_EnterStandUpdate(struct Player *);
void Player_UpdateFacingSide(struct Player *);
u32 Player_SetAnimation(struct Player *, u32, u32, u32);
void SpiralTrail_CreateWithParticles(struct Player *, s32, void *, s32, s32);
s32 Player_ConsumeStateEntry(struct Player *);
s32 ActorVariant_GetTableValue(s32, s32);
void Player_SetActionPhase(struct Player *, s32);
void Player_InitVisualResources(struct Player *);
void PlayerCue_UpdateFormChange(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        p->actionEighteenParameter = 0x16F;
        p->actionEighteenDuration = 0;
        p->actionEighteenEnabled = 1;
        Sound_PlaySong(0x306);
        if (p->actionMode != 1) {
            p->unk432[0] = 1;
            p->unk432[1] = 1;
        } else {
            p->unk432[0] = 0;
            p->unk432[1] = 1;
        }
    }
    p->statusValue = 4000;
    Player_AddControlFlags(p, 8);
    switch (p->tetherPhase) {
        case 0:
            if (((u8)(p->facing - 2)) <= 4) {
                p->facing = 4;
                p->heading = 4;
                p->side = 0;
            } else {
                p->facing = 0;
                p->heading = 0;
                p->side = 0;
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
            p->heading >>= 2;
            if (p->actionEighteenEnabled) {
                struct ContactPosition offset;
                offset.x = 0;
                offset.y = 0;
                offset.z = 15;
                SpiralTrail_CreateWithParticles(p, 1, &offset, 50, 69);
                Sound_PlaySong(0x39B);
            }
            Player_SetActionPhase(p, 1);
            break;

        case 1:
            if (p->actionEighteenEnabled) {
                if ((p->actionEighteenDuration += 3) > 32)
                    p->actionEighteenDuration = 32;
            }
            p->resetValue6FC = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 8), 64, 1)) {
                Player_SetActionPhase(p, 2);
                p->statHundredths = 0;
                p->statusValue = 4000;
            }
            break;

        case 2:
            if ((p->tetherFrame & 7) < (7 - (p->tetherFrame >> 2)))
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            else if (p->actionMode != 1) {
                Anim_SetKeyframes(&PlayerCue_Graphics(p)->primarySprite,
                                  &PlayerCue_Graphics(p)->primarySequence, 0xE4, 0, 0);
                Anim_Step(&PlayerCue_Graphics(p)->primarySprite,
                          &PlayerCue_Graphics(p)->primarySequence);
            } else
                Player_SetAnimation(p, ActorVariant_GetTableValue(0, 10), 64, 1);
            if ((p->actionEighteenDuration += 3) > 32)
                p->actionEighteenDuration = 32;
            p->resetValue6FC = 30;
            if ((++p->tetherFrame) > 19) {
                if (p->actionMode != 1)
                    p->actionMode = 1;
                else
                    p->actionMode = 0;
                Player_InitVisualResources(p);
                p->savedSpriteResource = 65535;
                if ((p->unk536 != 21) && (p->unk536 != 49))
                    p->active = 0;
                Player_SetActionPhase(p, 3);
            }
            break;

        case 3:
            p->resetValue6FC = 15;
            if (p->actionMode != 1) {
                if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 11), 64, 1)) {
                    Player_UpdateFacingSide(p);
                    ActorSequence_Begin(p, 0, 0);
                    Player_UpdateFacingSide(p);
                    p->actionLatch464 = 1;
                    p->actionLatch465 = 0;
                    Player_UpdateFacingSide(p);
                    PlayerCue_EnterStandUpdate(p);
                    p->pendingAnimationTrigger = 1;
                }
            } else {
                if (p->tetherFrame == 0)
                    Anim_SetKeyframes(&PlayerCue_Graphics(p)->primarySprite,
                                      &PlayerCue_Graphics(p)->primarySequence, 0xE4, 1, 0);
                Anim_Step(&PlayerCue_Graphics(p)->primarySprite,
                          &PlayerCue_Graphics(p)->primarySequence);
                if ((++p->tetherFrame) > 29) {
                    Player_UpdateFacingSide(p);
                    ActorSequence_Begin(p, 0, 0);
                    Player_UpdateFacingSide(p);
                    p->actionLatch464 = 1;
                    p->actionLatch465 = 1;
                    Player_UpdateFacingSide(p);
                    PlayerCue_EnterStandUpdate(p);
                    p->pendingAnimationTrigger = 1;
                }
            }
            break;
    }
}
