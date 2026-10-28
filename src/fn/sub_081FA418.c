#include "global.h"
#include "player.h"
// confidence=low: slot checks gate a two-stage transfer of the player to a sun-orb position.
struct Target {
    u32 distance;
    u8 pad[20];
};
struct Ratatosk {
    u8 pad[0xe00];
    struct Target targets[9];
};
#define H(o) (*(s16 *)(p + (o)))
#define U(o) (*(u16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
s32 SunOrb_GetSlotState(s32);
s32 Ratatosk_FindActiveTargetNearestPlayer(void *);
s32 SunOrb_IsFadingOut(s32);
s32 SlotController_IsSlotIdle(s32);
s32 Ratatosk_GetSelectedTargetIndex(void);
void *SunOrb_GetPositionFromManager(void *, s32);
void Player_ResetPositionState(void *, void *);
void Sound_PlaySong(u32);
s32 Ratatosk_FindActiveTargetNearestReference(void *);
void Player_ResetModePosition(void *, void *, s32);
s32 Ratatosk_GetSelectedPointIndex(void);
void SlotController_SetSlotFlag2(s32);
void RisingActorManager_BeginRaise(s32);
static inline u32 playerFlags(u8 *p, u32 mask)
{
    return *(u32 *)(p + 0x20) & mask;
}
void Ratatosk_UpdateSunOrbPlayerTransfer(u8 *p)
{
    s32 target;
    u8 *player;
    if (H(0xe7e) == 0 && H(0xe80) == 0 && SunOrb_GetSlotState(H(0xe4e)) <= 239 &&
        SunOrb_GetSlotState(H(0xe4e)) > 30 && (*(u8 **)gPlayers)) {
        target = Ratatosk_FindActiveTargetNearestPlayer(p);
        if (target < 0)
            return;
        if (SunOrb_IsFadingOut(H(0xe4e)))
            return;
        if (((struct Ratatosk *)p)->targets[target].distance <= 0x3fff &&
            SlotController_IsSlotIdle(target) && target == Ratatosk_GetSelectedTargetIndex() &&
            !*(u16 *)((player = (*(u8 **)gPlayers)) + 0x4d6) && !playerFlags(player, 0x40000)) {
            if (!H(0xe82)) {
                Player_ResetPositionState(
                    player, SunOrb_GetPositionFromManager(*(void **)(p + 0xf2c), target));
                H(0xe7e) = 90;
                Sound_PlaySong(0x572);
            }
            H(0xe82) = 1;
        } else
            H(0xe82) = 0;
    }
    if (H(0xe7e) > 0) {
        if (--H(0xe7e) == 0) {
            {
                s32 target = Ratatosk_FindActiveTargetNearestReference(p);
                if (target < 0)
                    target = Ratatosk_GetSelectedTargetIndex();
                player = (*(u8 **)gPlayers);
                if (player[0x457] == 26) {
                    Player_ResetModePosition(
                        player, SunOrb_GetPositionFromManager(*(void **)(p + 0xf2c), target), 150);
                    Sound_PlaySong(0x573);
                }
                H(0xe80) = 90;
            }
        }
    }
    if (H(0xe80) > 0) {
        SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
        RisingActorManager_BeginRaise(Ratatosk_GetSelectedPointIndex());
        H(0xe80)--;
    }
    if (H(0xe7e) > 0)
        SlotController_SetSlotFlag2(Ratatosk_GetSelectedTargetIndex());
}
