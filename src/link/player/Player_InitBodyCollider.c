#include "global.h"
#include "player/player.h"

void Collider_Init(void *, s32, s32, s32, s32, struct SceneVec *, struct SceneVec *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetPosition(void *, void *, s32);
void Player_ApplyGatedContact(void);
s32 Collider_Refresh(void *);

void Player_InitBodyCollider(struct Player *p)
{
    struct SceneVec size;
    struct SceneVec offset;
    struct PlayerCollider *collider = &p->collider;

    size.x = 50;
    size.y = 127;
    size.z = 50;
    offset.x = 0;
    offset.y = 127;
    offset.z = 0;
    Collider_Init(collider, p->positionalEffectGroup, 0x4001, 0, (u16)(1 << p->slot), &size,
                  &offset);
    Collider_SetCallback(collider, Player_ApplyGatedContact, p);
    Collider_SetPosition(collider, &p->colliderPosition, 0);
    Collider_Refresh(collider);
}
