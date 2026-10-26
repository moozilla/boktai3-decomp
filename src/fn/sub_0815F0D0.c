#define Player_RefreshCueCollider Player_UpdateColliderRefreshCountdown
#define gPlayerCueActive          gUnk_02000528
#include "global.h"
#include "player.h"
#include "game_state.h"
#include "ram.h"
extern u16 gUnk_02000528;
void PlayerCue_Dispatch(void *);
void Player_ResetStatus(void *);
void Player_RecordInputTicks(void *);
void Player_ConsumeActionCharge(void *);
void Player_UpdateColliderRefreshCountdown(void *);
s32 PlayerContact_ApplyTerrainDamage(void *);
void Player_EnterKindFour(void *);
void Player_UpdateResetBlink(void *);
void PlayerContact_UpdateThresholdEvent(void *);
void Player_ClearDirectionHistory(void *);
void Player_ApplyLatchedActionMode(void *);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void SceneActor_UpdateVisualEffects(void *);
void Player_RefreshVitalsAndCollision(void *);
void Player_SaveStateSnapshot(void *);
void sub_0821DE30(void *);
s32 Player_Update(struct Player *p)
{
    PlayerCue_Dispatch(p);
    Player_ResetStatus(p);
    if (p->kind == 1) {
        Player_RecordInputTicks(p);
        Player_ConsumeActionCharge(p);
        Player_UpdateColliderRefreshCountdown(p);
        if (gGameState->form != 2) {
            gPlayerCueTimer = 0;
            gUnk_02000528 = 0;
        }
        if (gUnk_02000528) {
            if (gPlayerCueTimer)
                gPlayerCueTimer--;
            else
                gUnk_02000528 = 0;
        }
        if (PlayerContact_ApplyTerrainDamage(p))
            Player_EnterKindFour(p);
        else if (p->tetherMode != 0x26) {
            Player_UpdateResetBlink(p);
            PlayerContact_UpdateThresholdEvent(p);
        }
    } else if (p->kind == 2) {
        Player_ClearDirectionHistory(p);
        if (p->currentStatA == 0)
            p->currentStatA = 1;
        p->particleEmissionCount = 0;
        p->directionFrame = 0;
        {
            s32 value = p->positionResetValue;
            if (value > 0)
                p->positionResetValue = 0;
        }
    }
    Player_ApplyLatchedActionMode(p);
    if (p->actionRequestState) {
        Sound_PlaySong(0x393);
        p->actionRequestState = 0;
    }
    Callback_InvokeActor(p, p->updateCallback);
    SceneActor_UpdateVisualEffects(p);
    Player_RefreshVitalsAndCollision(p);
    p->positionResetRequested = 0;
    if (!p->movementRequestBlocked)
        Player_SaveStateSnapshot(p);
    if (p->kind != 4) {
        p->trackedPosition = &p->position;
        sub_0821DE30(p->positionTracker);
    }
    return 0;
}
