#include "global.h"
#include "ram.h"

struct Rect {
    s16 x, size, y;
};
struct Camera {
    u8 pad[0x8C];
    u16 mode;
    u16 field8E, field90, field92, field94;
};
extern u8 gUnk_030042A0[];
void Camera_ApplyPositionBounds(struct Camera *, void *);
/* Camera mode 1 with limits centered on two ranges {center, size, center2}:
   center - size / 2 (+0x8E..+0x94), then Camera_ApplyPositionBounds (tentative). */
void Camera_SetCenteredBounds(struct Rect *a, struct Rect *b)
{
    struct Camera *c = gActiveCamera;
    if (c) {
        c->field8E = a->x - (a->size >> 1);
        c->field90 = a->y - (a->size >> 1);
        c->field92 = b->x - (b->size >> 1);
        c->field94 = b->y - (b->size >> 1);
        c->mode = 1;
        Camera_ApplyPositionBounds(c, gUnk_030042A0);
    }
}
