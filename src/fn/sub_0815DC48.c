#include "global.h"
#include "player.h"
#include "engine.h"
s32 Div(s32, s32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void PlayerCue_InitCollider(struct Player *p)
{
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
    Collider_SetAttackProperties(collider, Div(p->particleEmissionCount, 10), 40, 0, 0x40000, 10);
    Collider_SetCallback(collider, 0, p);
    Collider_SetPosition(collider, &p->position, 0);
    p->cueColliderPhase = 6;
    p->cueCollider.direction = ((p->facing + 1) & 7) << 5;
}
