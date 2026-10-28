#include "global.h"
#include "render.h"

struct Vec {
    s16 x, y, z;
};

struct Camera {
    u8 pad[0x1c];
    u16 f1c;
    u16 f1e;
    u8 pad2[0x68 - 0x20];
    u16 f68;
    u16 f6a;
    struct Vec pos;
};

extern struct Camera *gActiveCamera;
extern struct Vec gUnk_03005418;

static inline int project(int input)
{
    int v = input * 3;
    return v >= 0 ? v >> 5 : -(-v >> 5);
}

/* Camera command 1 (snap_to): move the camera to pos at once and recompute
   the scroll origin (gUnk_03005418) from its isometric projection. */
void Camera_SnapTo(struct Vec *pos)
{
    struct Camera *c = gActiveCamera;
    if (c) {
        c->pos.x = pos->x;
        c->pos.y = pos->y;
        c->pos.z = pos->z;
        gCamera.position.x = c->pos.x;
        gCamera.position.y = c->pos.y;
        gCamera.position.z = c->pos.z;
        {
            struct Vec *out = &gUnk_03005418;
            struct CameraPosition *in = &gCamera.position;
            int xy, height, x = in->x, z = in->z;
            out->x = project(x - z);
            xy = project(x + z);
            height = project(in->y);
            out->y = xy - height;
            out->z = xy + height;
        }
        gActiveCamera->f1c = 1;
        gActiveCamera->f68 = 1;
    }
}
