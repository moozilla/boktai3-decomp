#define Camera_SnapToIfActive Camera_SnapToWithoutCompletion
#define gCameraPosition       gCamera
#include "global.h"
#include "render.h"
#include "player.h"
s32 Div(s32, s32);
void Camera_SnapToWithoutCompletion(struct SceneVec *);
void Player_BlendCameraTarget(struct Player *p)
{
    s32 y, sum;
    p->cameraTarget.x = Div(p->position.x + p->cameraOffsetX + (gCamera.position.x << 2), 5);
    sum = p->position.y + p->cameraOffsetY;
    y = (gCamera.position.y << 2) + 150;
    p->cameraTarget.y = Div(sum + y, 5);
    p->cameraTarget.z = Div(p->position.z + p->cameraOffsetZ + (gCamera.position.z << 2), 5);
    Camera_SnapToWithoutCompletion(&p->cameraTarget);
}
