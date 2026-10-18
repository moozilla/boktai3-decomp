#include "global.h"

struct S080FE64C {
    u8 filler[0xdc];
    u8 *p;
};
extern u8 *gUnk_02000580;
void SceneActor_UpdateBehaviorCallbacks(struct S080FE64C *);

s32 Axe_OnRender(struct S080FE64C *s)
{
    s->p = gUnk_02000580 + 0x24;
    SceneActor_UpdateBehaviorCallbacks(s);
    return 0;
}
