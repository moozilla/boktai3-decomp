#define PooledInteractable_GetPosition NamedObject_GetPositionByName
#include "global.h"
#include "player.h"
struct SceneVec *NamedObject_GetPositionByName(s32);
s32 Player_TestMovementPosition(struct PlayerColliderVector *);
void Player_ClampInteractionVelocity(struct Player *p)
{
    struct PlayerColliderVector v;
    struct SceneVec *target;
    v.x = p->position.x + p->velocityX;
    v.z = p->position.z + p->velocityZ;
    if (p->tetherStatusFlags & 4) {
        target = NamedObject_GetPositionByName(p->tetherStatusTag);
        if ((s16)v.x <= target->x - 100)
            v.x = (u16)target->x - 100;
        else if ((s16)v.x >= target->x + 100)
            v.x = (u16)target->x + 100;
        if ((s16)v.z <= target->z - 100)
            v.z = (u16)target->z - 100;
        else if ((s16)v.z >= target->z + 100)
            v.z = (u16)target->z + 100;
        p->velocityX = v.x - p->position.x;
        p->velocityZ = v.z - p->position.z;
    } else if (Player_TestMovementPosition(&v)) {
        p->velocityX = 0;
        p->velocityZ = 0;
    }
}
