#include "global.h"
#include "render.h"
#include "ram.h"

struct Rect {
    u16 x, y, w;
};
struct Camera {
    u8 pad[0x8C];
    u16 mode;
    u16 field8E, field90, field92, field94;
};
void Camera_ApplyPositionBounds(struct Camera *, void *);
/* Camera mode 2 with limits from two ranges {start, ?, end} (+0x8E..+0x94),
   then Camera_ApplyPositionBounds (tentative). */
void Camera_SetBounds(struct Rect *a, struct Rect *b)
{
    struct Camera *c = gActiveCamera;
    if (c) {
        c->field8E = a->w - a->x;
        c->field90 = a->w + a->x - a->y;
        c->field92 = b->w - b->x;
        c->field94 = b->w + b->x - b->y;
        c->mode = 2;
        Camera_ApplyPositionBounds(c, &gCamera.position);
    }
}
