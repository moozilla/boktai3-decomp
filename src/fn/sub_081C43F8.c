#include "global.h"
// Provisional object-family interpretation.
#define SceneTransition_BeginVectorTween sub_081C43F8
struct SceneTransition { u8 pad[0x60]; void *target; void (*callback)(void); s32 timer; };
#define VectorTween_Initialize PositionMotion_InitTimedInterpolation
void PositionMotion_InitTimedInterpolation(void *, void *, s16 *, s16 *, u16);
void Registry6A37_UpdateSecondScrollTransition(void);
void sub_081C43F8(struct SceneTransition *p)
{
    s16 a[3], b[3];
    s16 *q = a;
    s32 x = 0xc0, y = 0x60;
    s32 zero = 0;
    a[0] = x; q[1] = y; q[2] = zero;
    { s16 *r = b; s32 x = 0xa7, y = 0x4b;
    r[0] = x; r[1] = y; r[2] = zero; }
    PositionMotion_InitTimedInterpolation((u8 *)p + 0x24, (u8 *)p + 0x1c, a, b, 0x5a);
    p->callback = Registry6A37_UpdateSecondScrollTransition;
    p->timer = zero;
}
