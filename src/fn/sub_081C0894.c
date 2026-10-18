#include "global.h"
#define ScreenTransition_BeginCornerMotion sub_081C0894
struct ScreenTransition {
    u8 unknown0[0x30];
    void *child;
    u8 unknown34[0x1c];
    void (*update)(void);
    u32 timer;
    s16 motionValue;
};
struct ScreenPoint { s16 x, y, z; };
void sub_081C32C8(void *, int, s16 *, int, int, int, int);
void SceneActorSequence_UpdateFirstWaypointMotion(void);
void sub_081C0894(struct ScreenTransition *p)
{
    struct ScreenPoint end;
    s16 *q = (s16 *)&end;
    int xy = 48, zero = 0;
    end.x = xy;
    q[1] = xy;
    q[2] = zero;
    sub_081C32C8(p->child, 0, (s16 *)&end, -1, xy, p->motionValue, 45);
    p->update = SceneActorSequence_UpdateFirstWaypointMotion;
    p->timer = zero;
}
