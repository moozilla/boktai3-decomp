#include "scene/camera/scene_camera_controller.h"
#include "global.h"
#include "engine/video/render.h"
#include "engine/engine.h"

void Camera_DispatchCommands(struct SceneCameraController *);
void sub_08226CF8(struct SceneCameraController *);
void Camera_UpdatePositionMode(struct SceneCameraController *);
void Camera_UpdateShake(struct SceneCameraController *);
void Camera_ApplyPositionBounds(struct SceneCameraController *, s16 *);

static inline s32 Scale(s32 v)
{
    s32 t = v * 3;
    s32 r;
    if (t >= 0)
        r = t >> 5;
    else
        r = -((-t) >> 5);
    return r;
}

u32 Camera_Update(struct SceneCameraController *c)
{
    s32 a, b, k, d;
    s16 *p, *out;
    if (!(gEngineFlags & 0x800)) {
        Camera_DispatchCommands(c);
    }
    if (!(gEnginePauseFlags & 4)) {
        sub_08226CF8(c);
        if (c->moving == 0) {
            gCamera.position.x = c->saved.x;
            gCamera.position.y = c->saved.y;
            gCamera.position.z = c->saved.z;
        } else {
            Camera_UpdatePositionMode(c);
        }
        Camera_UpdateShake(c);
        Camera_ApplyPositionBounds(c, (s16 *)&gCamera.position);
    }
    out = gCameraOrigin;
    p = (s16 *)&gCamera.position;
    a = p[0];
    b = p[2];
    out[0] = Scale(a - b);
    d = Scale(a + b);
    k = Scale(p[1]);
    out[1] = d - k;
    out[2] = d + k;
    return 0;
}
