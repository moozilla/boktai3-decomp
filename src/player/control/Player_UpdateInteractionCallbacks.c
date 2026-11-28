#include "global.h"
#include "player/player_status_flags.h"
#include "system/game_state/game_state.h"

extern void *gUnk_08610CA8[];
void Player_AccumulateAction(struct Player *, s32);
bool32 Player_CanUseSceneFeature(struct Player *);
void Player_UpdateTimedScriptEvent(struct Player *);
void Player_TickModeParticles(struct Player *);
void Player_UpdateDirectionHistory(struct Player *);
s32 Player_SelectInteractionMode(struct Player *);
void Player_UpdateCameraInput(struct Player *, s32);
void Callback_InvokeActor(struct Player *, void *);
void Player_ApplyDirectionalForce(struct Player *);
void Player_UpdateInteractionHoverDisplay(struct Player *);
void Player_BlendCameraTarget(struct Player *);
void Player_UpdateLockedMovement(struct Player *);
void Player_AddControlFlags(struct Player *, u32);
void PlayerCue_UpdatePeriodicParticles(struct Player *);

void Player_UpdateInteractionCallbacks(struct Player *p)
{
    Player_AddStatusFlags(p, 0x40000);
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
            mode = Player_SelectInteractionMode(p);
            Player_UpdateCameraInput(p, mode);
            Callback_InvokeActor(p, gUnk_08610CA8[mode]);
            if (p->movementVelocityLocked)
                *(struct SceneVec *)&p->velocityX = p->lockedVelocity;
            Player_ApplyDirectionalForce(p);
            Player_UpdateInteractionHoverDisplay(p);
            if (p->cameraOffsetActive)
                Player_BlendCameraTarget(p);
            Player_UpdateLockedMovement(p);
            break;
        }
        case 2:
            Player_AddControlFlags(p, 8);
            Callback_InvokeActor(p, p->triggerAnimationCallback);
            Player_UpdateInteractionHoverDisplay(p);
            PlayerCue_UpdatePeriodicParticles(p);
            break;
    }
}
