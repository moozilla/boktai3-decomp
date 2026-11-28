#include "global.h"
#include "bosses/count/scene_graphics_actor.h"

void ScaledShadow_Destroy(void *);
void Collider_Remove(void *);
void Anim_Remove(void *);
void SceneActors_Destroy(void *);

int Count_Destroy(struct SceneGraphicsActor *actor)
{
    ScaledShadow_Destroy(actor->shadow);
    Collider_Remove(&actor->overlayCollider);
    Anim_Remove(&actor->animation);
    SceneActors_Destroy(actor);
    return 0;
}
