#include "global.h"
struct P {
    s16 x;
    s16 p;
    s16 y;
};
struct Q {
    u8 p[0x20];
    s16 x;
    s16 pa;
    s16 y;
};
u8 Math_Atan2(s32, s32);
u32 Sqrt(u32);
void sub_082221F4(struct Q *q, struct P *pt, u8 *a, u16 *b)
{
    s32 dx = pt->x - q->x;
    s32 dy = pt->y - q->y;
    s32 d = dx * dx + dy * dy;
    if (d == 0) {
        *a = d;
        *b = d;
    } else {
        *a = Math_Atan2(dx, dy);
        *b = Sqrt(d);
    }
}
