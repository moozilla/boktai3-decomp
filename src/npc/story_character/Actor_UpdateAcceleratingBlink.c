#include "npc/manager/scene_actor_lifecycle.h"
u32 Actor_UpdateAcceleratingBlink(struct SceneActor *p, s32 b, s32 c)
{
    s32 n;
    if (b > (c * (c + 1)) >> 1)
        return 1;
    for (n = 0; n < c; n++) {
        b -= c - n;
        if (b <= 0)
            break;
    }
    if ((n & 1) == 0)
        SceneActor_SetFlags(p, 0x100);
    return 0;
}
