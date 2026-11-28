#include "global.h"
#include "player/player_status_flags.h"
#include "system/game_state/game_state.h"

extern void *gUnk_08610D18[];
void Player_AccumulateAction(struct Player *, s32);
bool32 Player_CanUseSceneFeature(struct Player *);
void Player_UpdateTimedScriptEvent(struct Player *);
void Player_TickModeParticles(struct Player *);
void Player_UpdateDirectionHistory(struct Player *);
s32 Player_SelectAlternateInteractionMode(struct Player *);
void Player_UpdateCameraInput(struct Player *, s32);
void Callback_InvokeActor(struct Player *, void *);
void Player_SynchronizeInteractionDisplay(struct Player *);
void Player_ClampPositionToStoredTerrainCell(struct Player *);
void Player_RequestNearbyEffectTarget(struct Player *);
void Player_UpdateInteractionHoverDisplay(struct Player *);
void Player_BlendCameraTarget(struct Player *);
void Player_UpdateLockedMovement(struct Player *);
void Player_AddControlFlags(struct Player *, u32);
void PlayerCue_UpdatePeriodicParticles(struct Player *);

void Player_UpdateAlternateInteractionCallbacks(struct Player *p)
{
    Player_AddStatusFlags(p, 0x54000);
    switch (p->kind) {
        case 1: {
            s32 mode;
            if (p->statusFlags & 0x10)
                Player_AccumulateAction(p, gGameState->unk876);
            if (Player_CanUseSceneFeature(p)) {
                Player_UpdateTimedScriptEvent(p);
                Player_TickModeParticles(p);
            }
            Player_UpdateDirectionHistory(p);
            mode = Player_SelectAlternateInteractionMode(p);
            Player_UpdateCameraInput(p, mode);
            Callback_InvokeActor(p, gUnk_08610D18[mode]);
            if (p->movementVelocityLocked)
                *(struct SceneVec *)&p->velocityX = p->lockedVelocity;
            Player_SynchronizeInteractionDisplay(p);
            if (p->cameraOffsetActive)
                Player_BlendCameraTarget(p);
            Player_ClampPositionToStoredTerrainCell(p);
            Player_RequestNearbyEffectTarget(p);
            Player_UpdateLockedMovement(p);
            break;
        }
        case 2:
            Player_AddControlFlags(p, 8);
            Callback_InvokeActor(p, p->triggerAnimationCallback);
            Player_SynchronizeInteractionDisplay(p);
            PlayerCue_UpdatePeriodicParticles(p);
            break;
    }
}
