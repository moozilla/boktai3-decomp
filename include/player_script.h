#ifndef GUARD_PLAYER_SCRIPT_H
#define GUARD_PLAYER_SCRIPT_H
#include "engine.h"
#include "player.h"
struct PlayerScriptItem {
    u16 unknown0, actionKind, unknown4, valueA, valueB;
    u8 unknownA[6];
};
struct PlayerScriptPackedPoint {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
extern const struct PlayerScriptItem gUnk_08E61280[];
extern const u16 gUnk_08610E24[];
s32 Script_GetActorIndex(void);
s32 Script_ReadPositionOption(s16 *);
void CellObject_Init(void *, void *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_StopActionSounds(struct Player *);
s32 Player_IsActiveInLocomotionState(struct Player *);
void sub_08159164(s32);
s32 Div(s32, s32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void sub_0813B3AC(struct Player *, s32);
u32 Player_GetEventFlags(struct Player *, u32);
u8 Player_IsEnchantmentAvailable(struct Player *, s32);
void Player_EmitQueuedEffects(struct Player *);
void sub_0813D834(struct Player *, s32, s32);
void Player_ClearStateTimer(struct Player *, s32);
void Player_EnterKindFour(struct Player *);
void Player_UpdateFacingSide(struct Player *);
#endif
