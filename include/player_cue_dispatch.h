#ifndef GUARD_PLAYER_CUE_DISPATCH_H
#define GUARD_PLAYER_CUE_DISPATCH_H
#include "player_cue_commands.h"
#include "scene_messages.h"

struct PlayerCuePacket {
    u16 id;
    u8 receiverKind, trigger;
    u16 unknown4;
    u8 kind, argumentCount;
    u16 arguments[16]; /* occupies the actor's complete 0x28-byte message buffer */
};

void PlayerCue_BeginAcknowledgement(struct Player *);
void PlayerCue_AcknowledgePendingTrigger(struct Player *);
s32 Div(s32, s32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
s32 Player_ScaleLevelValue(u32, s32);
void Player_OnStateOneEvent(u32, u32, struct Player *);
u32 SunClock_GetPhase(void);
void Collider_SetContactMode(void *, u32);
#endif
