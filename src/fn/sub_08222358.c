// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct Interp {
    u8 pad0[4];
    s32 x;
    s32 x0;
    s32 x1;
    s32 y;
    s32 y0;
    s32 y1;
    u16 t;
    u16 len;
};
s32 Div(s32, s32);
void Orbit_OffsetFromAngle(struct Interp *, void *, u32, u32);

u32 Interp_Step(struct Interp *o, void *p)
{
    if (o->len != 0) {
        o->t++;
        if (o->t <= o->len) {
            s32 rem;
            asm("" ::: "memory");
            rem = o->len - o->t;
            o->x = Div(o->x1 * rem + o->x0 * o->t, o->len);
            o->y = Div(o->y1 * rem + o->y0 * o->t, o->len);
            if (o->x < 0)
                o->x += 0x10000;
            o->x = *(u16 *)&o->x;
            Orbit_OffsetFromAngle(o, p, (u32)o->x >> 8, (u32)(o->y << 8) >> 16);
            return 0;
        }
    }
    return 1;
}
