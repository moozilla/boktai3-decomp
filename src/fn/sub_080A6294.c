#include "global.h"
/* TentacleBody_UpdateAnimationEleven: Starts directional animation 11 on state entry and completes when the ready flag is set */
#define TentacleBody_UpdateAnimationEleven sub_080A6294

void SceneActorAnimation_SelectDirectionalEntry(u8 *, u32);

static inline u8 Consume(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void sub_080A6294(u8 *p)
{
    if (Consume(p + 0x2b2))
        SceneActorAnimation_SelectDirectionalEntry(p, 11);
    if (p[0x2b3])
        p[0x2b0] = 1;
}
