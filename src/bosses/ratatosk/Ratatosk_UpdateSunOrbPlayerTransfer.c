#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "player/player.h"
// confidence=low: slot checks gate a two-stage transfer of the player to a sun-orb position.
s32 SunOrb_GetSlotState(s32);
s32 Ratatosk_FindActiveTargetNearestPlayer(struct Ratatosk *);
s32 SunOrb_IsFadingOut(s32);
s32 SlotController_IsSlotIdle(s32);
s32 Ratatosk_GetSelectedTargetIndex(void);
void *SunOrb_GetPositionFromManager(void *, s32);
void Player_ResetPositionState(void *, void *);
void Sound_PlaySong(u32);
s32 Ratatosk_FindActiveTargetNearestReference(struct Ratatosk *);
void Player_ResetModePosition(void *, void *, s32);
s32 Ratatosk_GetSelectedPointIndex(void);
void SlotController_SetSlotFlag2(s32);
void RisingActorManager_BeginRaise(s32);
static inline u32 playerFlags(struct Player *player, u32 mask)
{
    return player->statusFlags & mask;
}
void Ratatosk_UpdateSunOrbPlayerTransfer(struct Ratatosk *p)
{
    s32 target;
    struct Player *player;
    if (p->transferDelay == 0 && p->interruptLock == 0 &&
        SunOrb_GetSlotState(p->selectedTarget) <= 239 &&
        SunOrb_GetSlotState(p->selectedTarget) > 30 && gPlayers[0]) {
        target = Ratatosk_FindActiveTargetNearestPlayer(p);
        if (target < 0)
            return;
        if (SunOrb_IsFadingOut(p->selectedTarget))
            return;
        if (p->targets[target].distance <= 0x3fff && SlotController_IsSlotIdle(target) &&
            target == Ratatosk_GetSelectedTargetIndex() &&
            !(player = gPlayers[0])->positionResetRequested && !playerFlags(player, 0x40000)) {
            if (!p->transferArmed) {
                Player_ResetPositionState(player,
                                          SunOrb_GetPositionFromManager(p->sunOrbs, target));
                p->transferDelay = 90;
                Sound_PlaySong(0x572);
            }
            p->transferArmed = 1;
        } else
            p->transferArmed = 0;
    }
    if (p->transferDelay > 0) {
        if (--p->transferDelay == 0) {
            s32 nearest = Ratatosk_FindActiveTargetNearestReference(p);
            if (nearest < 0)
                nearest = Ratatosk_GetSelectedTargetIndex();
            player = gPlayers[0];
            if (player->tetherMode == 26) {
                Player_ResetModePosition(player, SunOrb_GetPositionFromManager(p->sunOrbs, nearest),
                                         150);
                Sound_PlaySong(0x573);
            }
            p->interruptLock = 90;
        }
    }
    if (p->interruptLock > 0) {
        SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
        RisingActorManager_BeginRaise(Ratatosk_GetSelectedPointIndex());
        p->interruptLock--;
    }
    if (p->transferDelay > 0)
        SlotController_SetSlotFlag2(Ratatosk_GetSelectedTargetIndex());
}
