#define Player_Update                sub_0815F0D0
#define Player_RefreshCueCollider Player_UpdateColliderRefreshCountdown
#define gPlayerCueTimer              gUnk_02000518
#define gPlayerCueActive             gUnk_02000528
#include "global.h"
#include "player.h"
#include "game_state.h"
extern u16 gUnk_02000518, gUnk_02000528;
void PlayerCue_Dispatch(void *);
void sub_0813B080(void *);
void sub_0815EF6C(void *);
void sub_0813ED20(void *);
void Player_UpdateColliderRefreshCountdown(void *);
s32 sub_0815E8A4(void *);
void sub_0815E790(void *);
void sub_0815EBB8(void *);
void sub_0815EFF8(void *);
void Player_ClearDirectionHistory(void *);
void sub_0815ED78(void *);
void Sound_PlaySong(u32);
s32 sub_0824923C(void *, void *);
void sub_0813DDAC(void *);
void Player_RefreshVitalsAndCollision(void *);
void sub_0815EC14(void *);
void sub_0821DE30(void *);
s32 sub_0815F0D0(struct Player *p)
{
    PlayerCue_Dispatch(p);
    sub_0813B080(p);
    if (p->kind == 1) {
        sub_0815EF6C(p);
        sub_0813ED20(p);
        Player_UpdateColliderRefreshCountdown(p);
        if (gUnk_02000710->form != 2) {
            gUnk_02000518 = 0;
            gUnk_02000528 = 0;
        }
        if (gUnk_02000528) {
            if (gUnk_02000518)
                gUnk_02000518--;
            else
                gUnk_02000528 = 0;
        }
        if (sub_0815E8A4(p))
            sub_0815E790(p);
        else if (p->tetherMode != 0x26) {
            sub_0815EBB8(p);
            sub_0815EFF8(p);
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
    sub_0815ED78(p);
    if (p->actionRequestState) {
        Sound_PlaySong(0x393);
        p->actionRequestState = 0;
    }
    sub_0824923C(p, p->updateCallback);
    sub_0813DDAC(p);
    Player_RefreshVitalsAndCollision(p);
    p->positionResetRequested = 0;
    if (!p->movementRequestBlocked)
        sub_0815EC14(p);
    if (p->kind != 4) {
        p->trackedPosition = &p->position;
        sub_0821DE30(p->positionTracker);
    }
    return 0;
}
