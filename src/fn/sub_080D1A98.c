#include "global.h"

#define SceneActorAnimation_UpdateDirectionalEntry sub_080D1A98

void SceneActorAnimation_SelectDirectionalEntry(struct SceneActorAnimationView *, s32);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void SceneActorAnimation_UpdateDirectionalEntry(u8 *p, s32 arg1)
{
    if (TakeFlag(p + 0x2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)p, 0);
        *(u8 *)(p + 0x2B3) = 0;
    }
    if (arg1 > 0x10 && *(u8 *)(p + 0x2B3) && *(u8 *)(p + 0x2B0) != 1) {
        *(u8 *)(p + 0x2B0) = 1;
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)p, 0);
    }
}
