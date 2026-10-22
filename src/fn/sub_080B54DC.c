#include "scene_actor_reset.h"
static inline u8 Test(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (actor->flags6D4 & mask)
        return 1;
    return 0;
}
// confidence=low: reports whether a response flag or nonnegative response distance is present.
#define Vorn_HasActiveResponse sub_080B54DC
u8 Vorn_HasActiveResponse(struct SceneActorRegistryResetView *context)
{
    if (!Test(context, 1) && context->field67C < 0)
        return 0;
    return 1;
}
