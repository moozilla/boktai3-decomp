#include "scene_arc.h"
s32 GroundEffect_Unlink(struct SceneGroundManager *manager, struct SceneGroundEffect *effect)
{
    struct SceneGroundEffect *prev = effect->prev, *next = effect->next;
    s32 result;
    if (!effect->active)
        return -1;
    if (prev)
        prev->next = next;
    else
        manager->head = next;
    if (next)
        next->prev = prev;
    result = 0;
    effect->active = result;
    return result;
}
