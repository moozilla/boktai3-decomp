#include "player_cue_sequence.h"
#include "game_state.h"
struct PlayerAlternateFormCue {
    u8 pad0[0x1D0];
    struct ContactSwarmSprite secondarySprite;
    u8 unknown1FC[0x1C];
    struct DisplaySequenceState secondarySequence;
    u32 secondarySequenceEnded;
    const void *secondaryResource;
    u8 unknown230[0x172];
    u8 heading;
    u8 side;
    u8 facing;
    u8 pad3A5[0x73];
    u8 actionMode;
    u8 pad419[0x17];
    u16 statusValue;
    u8 pad432[0x26];
    u8 phase;
    u8 pad459[0x1];
    u16 timer;
    u8 pad45C[0x8];
    u8 actionLatch;
    u8 actionLatchMode;
    u8 pad466[0x80];
    u16 visualOffsetStart;
    u8 pad4E8[0x2];
    u16 visualOffsetEnd;
    u8 pad4EC[0x2];
    u8 spiralEffectEnabled;
    u8 pad4EF[0x47];
    u16 nextSceneMode;
    u16 active;
    u8 pad53A[0x69];
    u8 pendingAnimationTrigger;
    u8 pad5A4[0x158];
    u16 effectStrength;
    u8 pad6FE[0x3C6];
    u16 savedSpriteResource;
    u8 padAC6[0x10];
    u16 spiralParameter;
    u16 spiralDuration;
};

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
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);

void PlayerCue_UpdateAlternateFormChange(struct PlayerAlternateFormCue *p)
{
    if (Player_ConsumeStateEntry(p)) {
        p->statusValue = 4000;
        p->spiralParameter = 0x16E;
        p->spiralDuration = 0;
        p->spiralEffectEnabled = 1;
        Sound_PlaySong(0x306);
        p->secondarySprite.flags &= ~1;
        p->secondarySprite.position.x = (u16)gGameState->positionX - 6;
        p->secondarySprite.position.y = 450 + (u16)gGameState->positionY;
        p->secondarySprite.position.z = (u16)gGameState->positionZ + 6;
        Anim_SetSequence(&p->secondarySequence, p->secondaryResource, 21, 0, 0);
        p->secondarySequenceEnded = 0;
    }
    if (p->secondarySequenceEnded)
        Anim_SetSequence(&p->secondarySequence, p->secondaryResource, 21, 0, 0);
    p->secondarySequenceEnded =
        PlayerCue_StepSequenceEnded(&p->secondarySprite, (void *)(&p->secondarySequence));
    if (p->secondarySequence.unknownA == 21) {
        if (p->secondarySprite.position.pad == 1)
            p->secondarySprite.position.y -= 10;
        else
            p->secondarySprite.position.y += 10;
    }
    if ((p->secondarySprite.position.pad == 1) &&
        (((s16)p->secondarySprite.position.y) < (gGameState->positionY + 220)))
        p->secondarySprite.position.y = (u16)gGameState->positionY + 220;
    switch (p->phase) {
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
            if (p->spiralEffectEnabled) {
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
            if (p->spiralEffectEnabled) {
                if ((p->spiralDuration += 3) > 32)
                    p->spiralDuration = 32;
            }
            p->effectStrength = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 8), 64, 1)) {
                if (p->spiralEffectEnabled)
                    Player_SetActionPhase(p, 2);
                else
                    Player_SetActionPhase(p, 3);
            }
            break;

        case 2: {
            if ((p->timer & 7) < (7 - (p->timer >> 2)))
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            else if (p->actionMode == 0)
                Player_SetAnimation(p, ActorVariant_GetTableValue(2, 10), 64, 1);
            else
                Player_SetAnimation(p, ActorVariant_GetTableValue(0, 10), 64, 1);
            if ((p->spiralDuration += 3) > 32)
                p->spiralDuration = 32;
            p->effectStrength = 30;
            if ((++p->timer) > 9) {
                if (!p->actionMode)
                    p->actionMode = 2;
                else
                    p->actionMode = 0;
                Player_InitVisualResources(p);
                p->savedSpriteResource = 0xFFFF;
                if ((p->nextSceneMode != 21) && (p->nextSceneMode != 49))
                    p->active = 0;
                Player_SetActionPhase(p, 4);
                p->secondarySprite.flags |= 1;
            }
            break;
        }

        case 3:
            if (!p->timer)
                Sound_PlaySong(0x192);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            p->effectStrength = 30;
            if ((++p->timer) > 15)
                Player_SetActionPhase(p, 4);
            break;

        case 4:
            p->effectStrength = 15;
            ++p->timer;
            if (p->actionMode != 2) {
                if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 11), 64, 1)) {
                    Player_UpdateFacingSide(p);
                    ActorSequence_Begin(p, 0, 0);
                    break;
                }
            } else {
                p->visualOffsetStart = 0xFFF4;
                p->visualOffsetEnd = 12;
                Anim_SetKeyframes(((u8 *)p) + 0x8C, ((u8 *)p) + 0x6C, 0x1AB, 1, 0);
                Anim_Step(((u8 *)p) + 0x8C, ((u8 *)p) + 0x6C);
            }
            Player_InitVisualResources(p);
            p->savedSpriteResource = 0xFFFF;
            p->spiralDuration = 0;
            if ((++p->timer) > 29) {
                Player_UpdateFacingSide(p);
                p->actionLatch = 1;
                p->actionLatchMode = 2;
                Player_UpdateFacingSide(p);
                PlayerCue_EnterStandUpdate(p);
                p->pendingAnimationTrigger = 1;
            }
            break;
    }
}
