#include "global.h"
#include "ram.h"

struct Vec {
    s16 x, y, z;
};

struct Camera {
    u8 pad[0x1c];
    u16 f1c;
    u8 pad1e[0x54 - 0x1e];
    u16 f54;
    u16 f56;
    u8 pad58[0x60 - 0x58];
    u16 x, y, z;
    u8 pad66[0x80 - 0x66];
    u16 f80;
    u16 f82;
};

extern struct Vec gUnk_030042A0;
extern struct Vec gUnk_03005418;

void Camera_ApplyPositionBounds(struct Camera *, u16 *);

static inline int project(int input)
{
    int v = input * 3;
    return v >= 0 ? v >> 5 : -(-v >> 5);
}

static inline void updateScroll(void)
{
    struct Vec *out = &gUnk_03005418;
    struct Vec *in = &gUnk_030042A0;
    int xy, height, x = in->x, z = in->z;
    out->x = project(x - z);
    xy = project(x + z);
    height = project(in->y);
    out->y = xy - height;
    out->z = xy + height;
}

/* Move the camera to pos at once (clamped), and when it follows no target
   (f1c == 0) refresh the scroll position from it plus the offset at 0x80. */
void Camera_SetPosition(u16 *pos)
{
    struct Camera *c = gActiveCamera;
    if (c) {
        c->x = pos[0];
        c->y = pos[1];
        c->z = pos[2];
        Camera_ApplyPositionBounds(c, &c->x);
        c = gActiveCamera;
        c->f54 = 1;
        if (c->f1c == 0) {
            gUnk_030042A0.x = c->x + c->f80;
            gUnk_030042A0.y = c->y;
            gUnk_030042A0.z = c->z + c->f82;
            updateScroll();
        }
    }
}
