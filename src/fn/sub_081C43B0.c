#include "global.h"
// Provisional object-family interpretation.
#define SceneTransition_BeginOffsetMove sub_081C43B0
struct SceneTransition { u8 pad[0x60]; void *target; void (*callback)(void); s32 timer; };
void sub_081C371C(void *, s16 *, s16 *, s32, s32, s32);
void Registry6A37_UpdateDisplayHold(void);
void sub_081C43B0(struct SceneTransition *p)
{
    s16 a[3], b[3];
    s16 *q = a;
    s32 x = 0x58, y = -0x10;
    s32 zero = 0;
    a[0] = x; q[1] = y; q[2] = zero;
    { s16 *r = b; s32 x = 0x38, y = 0x30;
    r[0] = x; r[1] = y; r[2] = zero; }
    sub_081C371C(p->target, (s16 *)&a, (s16 *)&b, 0x18, 1, 0x78);
    p->callback = Registry6A37_UpdateDisplayHold;
    p->timer = zero;
}
