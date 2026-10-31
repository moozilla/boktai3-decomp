#include "player_cue.h"
#include "player_cue_sequence.h"
#include "game_state.h"

s32 Player_ConsumeStateEntry(void *);
void Player_SetActionPhase(void *, s32);
u32 Player_SetAnimation(void *, u32, u32, u32);
s32 ActorVariant_GetTableValue(s32, s32);
void Player_UpdateFacingSide(void *);
void Player_InitVisualResources(void *);
void ActorSequence_Begin(void *, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void SpiralTrail_CreateWithParticles(void *, s32, void *, s32, s32);
void Sound_PlaySong(u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);

void PlayerCue_UpdateAlternateFormChange(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        p->statusValue = 4000;
        p->actionEighteenParameter = 0x16E;
        p->actionEighteenDuration = 0;
        p->actionEighteenEnabled = 1;
        Sound_PlaySong(0x306);
        PlayerCue_Auxiliary(p)->sprite.flags &= ~1;
        PlayerCue_Auxiliary(p)->sprite.position.x = (u16)gGameState->positionX - 6;
        PlayerCue_Auxiliary(p)->sprite.position.y = 450 + (u16)gGameState->positionY;
        PlayerCue_Auxiliary(p)->sprite.position.z = (u16)gGameState->positionZ + 6;
        Anim_SetSequence(&PlayerCue_Auxiliary(p)->sequence, PlayerCue_Auxiliary(p)->resource, 21, 0,
                         0);
        PlayerCue_Auxiliary(p)->sequenceEnded = 0;
    }
    if (PlayerCue_Auxiliary(p)->sequenceEnded)
        Anim_SetSequence(&PlayerCue_Auxiliary(p)->sequence, PlayerCue_Auxiliary(p)->resource, 21, 0,
                         0);
    PlayerCue_Auxiliary(p)->sequenceEnded = PlayerCue_StepSequenceEnded(
        &PlayerCue_Auxiliary(p)->sprite, (void *)(&PlayerCue_Auxiliary(p)->sequence));
    if (PlayerCue_Auxiliary(p)->sequence.unknownA == 21) {
        if (PlayerCue_Auxiliary(p)->sprite.position.pad == 1)
            PlayerCue_Auxiliary(p)->sprite.position.y -= 10;
        else
            PlayerCue_Auxiliary(p)->sprite.position.y += 10;
    }
    if ((PlayerCue_Auxiliary(p)->sprite.position.pad == 1) &&
        (((s16)PlayerCue_Auxiliary(p)->sprite.position.y) < (gGameState->positionY + 220)))
        PlayerCue_Auxiliary(p)->sprite.position.y = (u16)gGameState->positionY + 220;
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
                struct ContactPackedVector particles;
                particles.x = 0;
                particles.y = 0;
                particles.z = 15;
                SpiralTrail_CreateWithParticles(p, 0, &particles, 50, 50);
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
                if (p->actionEighteenEnabled)
                    Player_SetActionPhase(p, 2);
                else
                    Player_SetActionPhase(p, 3);
            }
            break;

        case 2: {
            if ((p->tetherFrame & 7) < (7 - (p->tetherFrame >> 2)))
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            else if (p->actionMode == 0)
                Player_SetAnimation(p, ActorVariant_GetTableValue(2, 10), 64, 1);
            else
                Player_SetAnimation(p, ActorVariant_GetTableValue(0, 10), 64, 1);
            if ((p->actionEighteenDuration += 3) > 32)
                p->actionEighteenDuration = 32;
            p->resetValue6FC = 30;
            if ((++p->tetherFrame) > 9) {
                if (!p->actionMode)
                    p->actionMode = 2;
                else
                    p->actionMode = 0;
                Player_InitVisualResources(p);
                p->savedSpriteResource = 0xFFFF;
                if ((p->unk536 != 21) && (p->unk536 != 49))
                    p->active = 0;
                Player_SetActionPhase(p, 4);
                PlayerCue_Auxiliary(p)->sprite.flags |= 1;
            }
            break;
        }

        case 3:
            if (!p->tetherFrame)
                Sound_PlaySong(0x192);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            p->resetValue6FC = 30;
            if ((++p->tetherFrame) > 15)
                Player_SetActionPhase(p, 4);
            break;

        case 4:
            p->resetValue6FC = 15;
            ++p->tetherFrame;
            if (p->actionMode != 2) {
                if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 11), 64, 1)) {
                    Player_UpdateFacingSide(p);
                    ActorSequence_Begin(p, 0, 0);
                    break;
                }
            } else {
                PlayerCue_Graphics(p)->visualOffsetStart = 0xFFF4;
                PlayerCue_Graphics(p)->visualOffsetEnd = 12;
                Anim_SetKeyframes(&p->animation, p->animationData, 0x1AB, 1, 0);
                Anim_Step(&p->animation, p->animationData);
            }
            Player_InitVisualResources(p);
            p->savedSpriteResource = 0xFFFF;
            p->actionEighteenDuration = 0;
            if ((++p->tetherFrame) > 29) {
                Player_UpdateFacingSide(p);
                p->actionLatch464 = 1;
                p->actionLatch465 = 2;
                Player_UpdateFacingSide(p);
                PlayerCue_EnterStandUpdate(p);
                p->pendingAnimationTrigger = 1;
            }
            break;
    }
}
