#include "global.h"
#include "player/player.h"
#include "scene/collision/scene_actor_damage_visibility.h"

s32 Div(s32, s32);
void Camera_SnapToWithoutCompletion(void *);

// confidence=low: smooths the camera anchor toward actor position and motion offset.
void Player_SmoothCameraAnchor(struct Player *p)
{
    p->cameraTarget.x = Div(p->position.x + p->cameraOffsetX + p->cameraTarget.x * 4, 5);
    {
        s32 sum = p->position.y + p->cameraOffsetY;
        s32 previous = p->cameraTarget.y * 4;

        previous += 150;
        p->cameraTarget.y = Div(sum + previous, 5);
    }
    p->cameraTarget.z = Div(p->position.z + p->cameraOffsetZ + p->cameraTarget.z * 4, 5);
    if (p->movementRequestBlocked == SceneActorDamage_LocalPlayer())
        Camera_SnapToWithoutCompletion(&p->cameraTarget);
}
