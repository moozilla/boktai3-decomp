#include "scene_actor_registry.h"
void SceneActorRegistry_MarkCategoryOne(u8 category)
{
    if (category == 1)
        gGameState->flags868 |= category;
}
