#include "global.h"
void SceneTarget_BeginMove(void *, int, s16 *, int, int, int, int);
void SceneActorSequence_UpdateSecondWaypointMotion(void);
void SceneActorSequence_BeginSecondWaypointMotion(u8 *p)
{
    struct V {
        s16 x, y, z;
    } v;
    s16 *q = (s16 *)&v;
    int a = 0x70, b = 0x48, z = 0;
    v.x = a;
    q[1] = b;
    q[2] = z;
    SceneTarget_BeginMove(*(void **)(p + 0x30), 0, (s16 *)&v, -1, 0x10, *(s16 *)(p + 0x5a), 0x32);
    *(void (**)(void))(p + 0x50) = SceneActorSequence_UpdateSecondWaypointMotion;
    *(u32 *)(p + 0x54) = z;
}
