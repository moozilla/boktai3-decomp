#include "engine/shared/global.h"
#include "engine/scene_body/scene_body_registry.h"
u32 SceneBody_InitRegistry(struct SceneBodyRegistry *p)
{
    gSceneBodyRegistry = p;
    p->first = 0;
    p->last = 0;
    return 0;
}
