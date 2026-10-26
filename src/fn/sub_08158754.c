#include "global.h"
extern u8 *gGameState;
extern void *gUnk_08610D18[];
void Player_AccumulateAction(u8 *, s32);
bool32 Player_CanUseSceneFeature(u8 *);
void Player_UpdateTimedScriptEvent(u8 *);
void Player_TickModeParticles(u8 *);
void Player_UpdateDirectionHistory(u8 *);
s32 Player_SelectAlternateInteractionMode(u8 *);
void Player_UpdateCameraInput(u8 *, s32);
void Callback_InvokeActor(u8 *, void *);
void Player_SynchronizeInteractionDisplay(u8 *);
void Player_ClampPositionToStoredTerrainCell(u8 *);
void Player_RequestNearbyEffectTarget(u8 *);
void Player_UpdateInteractionHoverDisplay(u8 *);
void Player_BlendCameraTarget(u8 *);
void Player_UpdateLockedMovement(u8 *);
void Player_AddControlFlags(u8 *, u32);
void PlayerCue_UpdatePeriodicParticles(u8 *);
struct Vec2 {
    s32 x, y;
};

void sub_08158754(u8 *p)
{
    u32 flags;
    u32 bit = 0x54000;
    flags = *(u32 *)(p + 0x20);
    flags |= bit;
    *(u32 *)(p + 0x20) = flags;
    switch (*(s32 *)(p + 0x1c)) {
        case 1: {
            s32 mode;
            if (flags & 0x10)
                Player_AccumulateAction(p, *(s16 *)(gGameState + 0x876));
            if (Player_CanUseSceneFeature(p)) {
                Player_UpdateTimedScriptEvent(p);
                Player_TickModeParticles(p);
            }
            Player_UpdateDirectionHistory(p);
            mode = Player_SelectAlternateInteractionMode(p);
            Player_UpdateCameraInput(p, mode);
            Callback_InvokeActor(p, gUnk_08610D18[mode]);
            if (p[0x48c])
                *(struct Vec2 *)(p + 0x38) = *(struct Vec2 *)(p + 0x490);
            Player_SynchronizeInteractionDisplay(p);
            if (*(s16 *)(p + 0x4b6))
                Player_BlendCameraTarget(p);
            Player_ClampPositionToStoredTerrainCell(p);
            Player_RequestNearbyEffectTarget(p);
            Player_UpdateLockedMovement(p);
            break;
        }
        case 2:
            Player_AddControlFlags(p, 8);
            Callback_InvokeActor(p, *(void **)(p + 0x58c));
            Player_SynchronizeInteractionDisplay(p);
            PlayerCue_UpdatePeriodicParticles(p);
            break;
    }
}
