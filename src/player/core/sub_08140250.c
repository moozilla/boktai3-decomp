#include "global.h"
#include "player/player.h"
#include "scene/collision/contact_position.h"

// confidence=low: sets up the player body collider that grants stat rewards on contact.
void Collider_Init(struct PlayerCollider *, s32, s32, s32, s32, struct ContactPosition *,
                   struct ContactPosition *);
void Collider_SetBodyProperties(struct PlayerCollider *, s32, s32, s32);
void Collider_SetCallback(struct PlayerCollider *, void *, struct Player *);
void Collider_SetPosition(struct PlayerCollider *, struct SceneVec *, s32);
void PlayerContact_ApplyStatReward(void);
void PlayerAction_RefreshModifiers(struct Player *);
s32 Collider_Refresh(struct PlayerCollider *);

void sub_08140250(struct Player *p)
{
    struct ContactPosition scale;
    struct ContactPosition position;
    struct PlayerCollider *collider = &p->collider;
    scale.x = 50;
    scale.y = 127;
    scale.z = 50;
    position.x = 0;
    position.y = 127;
    position.z = 0;
    Collider_Init(collider, p->positionalEffectGroup, 0x5001, 0, (u16)(1 << p->slot), &scale,
                  &position);
    switch (p->actionMode) {
        case 0:
        case 2:
            Collider_SetBodyProperties(collider, 0x14, 1, 2);
            break;
        case 1:
        case 6:
            Collider_SetBodyProperties(collider, 0x14, 2, 1);
            break;
        default:
            Collider_SetBodyProperties(collider, 0x14, 1, 2);
            break;
    }
    Collider_SetCallback(collider, PlayerContact_ApplyStatReward, p);
    Collider_SetPosition(collider, &p->colliderPosition, 0);
    Collider_Refresh(collider);
    PlayerAction_RefreshModifiers(p);
}
