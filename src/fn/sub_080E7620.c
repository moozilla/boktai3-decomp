#include "global.h"

u32 SceneActor_GetDirectionFlags(u32);
void SceneActorAnimation_SelectEntryFlags(void *, u32, u32);

struct W {
    u16 pad[3];
    u16 f;
};

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void Mimic_OnAnimationStateChanged(u8 *p)
{
    if (TakeFlag(p + 0x2B2)) {
        u32 r = SceneActor_GetDirectionFlags(*(u8 *)(p + 0x59));
        struct W *w;
        SceneActorAnimation_SelectEntryFlags(p, 0, r);
        w = (struct W *)(p + 0x188);
        w->f = 4 | w->f;
    }
}
