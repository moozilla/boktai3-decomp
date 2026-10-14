#include "global.h"

struct P {
    s16 x;
    s16 pad;
    s16 y;
};
struct A {
    u8 f[0x20];
    s16 x;
    s16 pad;
    s16 y;
};
struct P *TetherActor_GetPosition(void);
s32 Math_Atan2(s32, s32);
void TetherActor_RequestKindNineAction(s32, s32);

void sub_081A4B80(struct A *p)
{
    struct P *q = TetherActor_GetPosition();
    s32 dx = q->x - p->x;
    s32 dy = q->y - p->y;
    if (dx * dx + dy * dy <= 0xFFFF)
        TetherActor_RequestKindNineAction(0x80, Math_Atan2(dx, dy));
}
