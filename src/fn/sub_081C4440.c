#include "global.h"
// Provisional object-family interpretation.
struct SceneTransition {
    u8 pad[0x60];
    void *target;
    void (*callback)(void);
    s32 timer;
    s32 f6c;
};
void PositionMotion_InitTimedInterpolation(void *, void *, s16 *, s16 *, u16);
void sub_081C42B0(s32, s32, s32);
void SceneTransition_SpawnParticleBurst(s32, s32, s32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Registry6A37_UpdateFirstScrollTransition(void);

void SceneTransition_BeginFirstScroll(struct SceneTransition *p)
{
    s16 a[3], b[3];
    s16 *q = a;
    s32 x = 0x15e, y = 0xc4;
    s32 zero = 0;
    a[0] = x;
    q[1] = y;
    q[2] = zero;
    {
        s16 *r = b;
        s32 x = 0xc0, y = 0x60;
        r[0] = x;
        r[1] = y;
        r[2] = zero;
    }
    PositionMotion_InitTimedInterpolation((u8 *)p + 0x24, (u8 *)p + 0x1c, a, b, 0xf);
    for (zero = 0; zero <= 7; zero++) {
        sub_081C42B0(0x50, 0x30, zero);
    }
    SceneTransition_SpawnParticleBurst(0, 0, p->f6c);
    {
        s32 zero;
        ScreenTransition_SendCommand(0, 4, 0x1f, 0x1f, 0x1f, 0xFFFF, zero = 0);
        p->callback = Registry6A37_UpdateFirstScrollTransition;
        p->timer = zero;
    }
}
