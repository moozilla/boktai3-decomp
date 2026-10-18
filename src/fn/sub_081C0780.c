#include "global.h"
/* Tentative: moves the child from (120,80,0) to (120,144,0). */
struct ScreenTransition {
    u8 unknown0[0x30];
    void *child;
    u8 unknown34[0x1c];
    void (*update)(void);
    u32 timer;
};
struct ScreenPoint {
    s16 x, y, z;
};
void SceneTarget_BeginMove(void *, struct ScreenPoint *, struct ScreenPoint *, int, int, int, int);
void sub_081C07C8(void);
void ScreenTransition_BeginDownwardMotion(struct ScreenTransition *p)
{
    struct ScreenPoint start, end;
    s16 *q = (s16 *)&start;
    int x = 120, y = 80, zero = 0;
    start.x = x;
    q[1] = y;
    q[2] = zero;
    {
        s16 *r = (s16 *)&end;
        int y2 = 144;
        r[0] = x;
        r[1] = y2;
        r[2] = zero;
    }
    SceneTarget_BeginMove(p->child, &start, &end, 1, 64, 128, 60);
    p->update = sub_081C07C8;
    p->timer = zero;
}
