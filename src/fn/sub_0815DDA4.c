#include "global.h"
#include "player.h"
#include "engine.h"
#include "game_state.h"

#define PlayerCue_ComputeColliderPower  Player_ScaleLevelValue
#define PlayerCue_RecordColliderContact Player_OnStateOneEvent
s32 Player_ScaleLevelValue(u32, s32);
void Player_OnStateOneEvent(u32, u32, struct Player *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void PlayerCue_InitAttackCollider(struct Player *p, void *position, u32 flags, s32 scale)
{
    s32 sum;
    struct PlayerCueCollider *collider = &p->cueCollider;
    struct PlayerColliderVector size;
    struct PlayerColliderVector offset;
    size.x = 120;
    size.y = 120;
    size.z = 120;
    offset.x = 0;
    offset.y = 120;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, (0x10000u << p->movementRequestBlocked) >> 16, &size,
                  &offset);
    if (gEngineFlags & 0x1000) {
        s32 bonus = gGameState->unk874;
        s32 level = gGameState->level;
        bonus += 50;
        sum = level + bonus;
        Collider_SetAttackProperties(collider, sum, 40, 16, 0x40000, 10);
    } else
        Collider_SetAttackProperties(collider, Player_ScaleLevelValue(flags, scale), 40, 1,
                                     flags | 0x40000, 10);
    Collider_SetCallback(collider, Player_OnStateOneEvent, p);
    Collider_SetPosition(collider, position, 0);
    p->cueColliderPhase = 6;
    sum = (p->facing + 1) & 7;
    p->cueCollider.direction = sum << 5;
}
