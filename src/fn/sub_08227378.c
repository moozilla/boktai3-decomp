#include "global.h"
#include "render.h"
#include "engine.h"

struct Camera {
    u8 pad0[0x1c];
    u16 f1C;
    u8 pad1E[0x3a];
    u16 f58;
    u16 f5A;
    u16 f5C;
};
void Camera_DispatchCommands(struct Camera *);
void sub_08226CF8(struct Camera *);
void Camera_UpdatePositionMode(struct Camera *);
void Camera_UpdateShake(struct Camera *);
void Camera_ApplyPositionBounds(struct Camera *, s16 *);

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

u32 Camera_Update(struct Camera *c)
{
    s32 a, b, k, d;
    s16 *p, *out;
    if (!(gEngineFlags & 0x800)) {
        Camera_DispatchCommands(c);
    }
    if (!(gEnginePauseFlags & 4)) {
        sub_08226CF8(c);
        if (c->f1C == 0) {
            gCamera.position.x = c->f58;
            gCamera.position.y = c->f5A;
            gCamera.position.z = c->f5C;
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
