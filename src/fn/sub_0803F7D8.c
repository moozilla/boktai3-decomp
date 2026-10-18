#include "scene_arc.h"
struct SceneGroundManager *ResourceRegistry_GetResource(u32);
s32 GroundEffect_Unlink(struct SceneGroundManager *, struct SceneGroundEffect *);
u32 GroundEffect_Destroy(struct SceneGroundEffect *effect)
{
    struct SceneGroundManager *manager = ResourceRegistry_GetResource(0xf0de);
    Effect_Remove(&effect->flags);
    if (manager)
        GroundEffect_Unlink(manager, effect);
    return 0;
}
