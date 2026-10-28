#include "global.h"
#include "player.h"

struct S080FE64C {
    u8 filler[0xdc];
    u8 *p;
};
void SceneActor_UpdateBehaviorCallbacks(struct S080FE64C *);

s32 Axe_OnRender(struct S080FE64C *s)
{
    s->p = (*(u8 **)gPlayers) + 0x24;
    SceneActor_UpdateBehaviorCallbacks(s);
    return 0;
}
