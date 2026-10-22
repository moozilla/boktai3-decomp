#include "scene_actor_reset.h"
u32 SceneActor_DispatchContextAction(struct SceneActorRegistryResetView *, u32);
void SceneActorAnimation_SelectDirectionalEntry(struct SceneActorRegistryResetView *, u32);
// confidence=low: enters directional animation three when the contact-ready flag is set.
#define Vorn_BeginContactAnimation sub_080B3BD0
u8 Vorn_BeginContactAnimation(struct SceneActorRegistryResetView *actor)
{
    if (actor->field5CA == 0 && (actor->field2E6 & 8)) {
        actor->field2B8 = 45;
        actor->field5CA = 65;
        SceneActorAnimation_SelectDirectionalEntry(actor, 3);
    }
    return SceneActor_DispatchContextAction(actor, 0);
}
