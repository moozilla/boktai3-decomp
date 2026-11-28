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
void Math_GetTargetAngleAndDistance(struct Q *q, struct P *pt, u8 *angle, u16 *distance)
{
    s32 dx = pt->x - q->x;
    s32 dy = pt->y - q->y;
    s32 squared = dx * dx + dy * dy;
    if (squared == 0) {
        *angle = 0;
        *distance = 0;
    } else {
        *angle = Math_Atan2(dx, dy);
        *distance = Sqrt(squared);
    }
}
