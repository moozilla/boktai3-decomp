#include "global.h"

struct Rect {
    u16 x, y, w;
};
struct Camera {
    u8 pad[0x8C];
    u16 mode;
    u16 field8E, field90, field92, field94;
};
extern struct Camera *gUnk_03005420;
extern u8 gUnk_030042A0[];
void sub_08225F1C(struct Camera *, void *);
/* Camera mode 2 with limits from two ranges {start, ?, end} (+0x8E..+0x94),
   then sub_08225F1C (tentative). */
void Camera_SetBounds(struct Rect *a, struct Rect *b)
{
    struct Camera *c = gUnk_03005420;
    if (c) {
        c->field8E = a->w - a->x;
        c->field90 = a->w + a->x - a->y;
        c->field92 = b->w - b->x;
        c->field94 = b->w + b->x - b->y;
        c->mode = 2;
        sub_08225F1C(c, gUnk_030042A0);
    }
}
