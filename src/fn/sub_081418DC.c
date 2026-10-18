#define Player_BlendCameraTarget sub_081418DC
#define Camera_SnapToIfActive Camera_SnapToWithoutCompletion
#define gCameraPosition          gUnk_030042A0
#include "global.h"
#include "player.h"
extern struct SceneVec gUnk_030042A0;
s32 Div(s32, s32);
void Camera_SnapToWithoutCompletion(struct SceneVec *);
void sub_081418DC(struct Player *p)
{
    s32 y, sum;
    p->cameraTarget.x = Div(p->position.x + p->cameraOffsetX + (gUnk_030042A0.x << 2), 5);
    sum = p->position.y + p->cameraOffsetY;
    y = (gUnk_030042A0.y << 2) + 150;
    p->cameraTarget.y = Div(sum + y, 5);
    p->cameraTarget.z = Div(p->position.z + p->cameraOffsetZ + (gUnk_030042A0.z << 2), 5);
    Camera_SnapToWithoutCompletion(&p->cameraTarget);
}
