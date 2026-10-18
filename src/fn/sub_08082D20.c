#include "global.h"
/* SceneActorControl_UpdateTimedResponseAnimation: Selects heading-dependent animation 11 and sets response flags at the 30 and 60 tick thresholds; response interpretation is provisional. */
#define SceneActorControl_UpdateTimedResponseAnimation sub_08082D20

void SceneActorControl_GetDirectionalVariant(u32, s32 *, s32 *);
void SceneActorAnimation_SelectExplicitVariant(u8 *, u32, s32, s32);

static inline void Or16(u16 m, u16 *p)
{
    *p |= m;
}

void sub_08082D20(u8 *p, s32 time)
{
    s32 a, b;
    if (p[0x2b2])
        p[0x2b2] = 0;
    SceneActorControl_GetDirectionalVariant(p[0x59], &a, &b);
    SceneActorAnimation_SelectExplicitVariant(p, 11, a, b);
    if (time <= 29)
        Or16(0x20, (u16 *)(p + 0x2d6));
    if (time > 59)
        p[0x2b0] = 1;
}
