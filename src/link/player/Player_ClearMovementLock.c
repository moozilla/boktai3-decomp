#include "global.h"
#include "player/player.h"
#include "scene/collision/scene_actor_damage_visibility.h"

void Camera_FollowPlayerWithoutCompletion(u32);

void Player_ClearMovementLock(struct Player *p)
{
    if (p->slot == SceneActorDamage_LocalPlayer())
        Camera_FollowPlayerWithoutCompletion(8);
    p->cameraOffsetActive = 0;
}
