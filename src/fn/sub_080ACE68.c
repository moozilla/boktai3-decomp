#define Vorn_UpdateScriptedAnimation sub_080ACE68
#include "global.h"

void SceneActorAnimation_SelectDirectionalEntry(void *, u32);
void SceneActor_RunPositionScript(void *);

struct VornAnimation {
    u8 unknown0[0xC];
    u16 frame;
    u8 unknownE[6];
    u16 state;
};

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void Vorn_UpdateScriptedAnimation(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
        SceneActorAnimation_SelectDirectionalEntry(p, 0xF);
    {
        struct VornAnimation *anim = (struct VornAnimation *)(*(u8 **)(p + 4) + 0x20);
        if (anim->state == 3 && anim->frame == 0)
            SceneActor_RunPositionScript(*(void **)(p + 0x3D0));
    }
    if (*(u8 *)(p + 0x2B3))
        *(u8 *)(p + 0x2B0) = 1;
}
