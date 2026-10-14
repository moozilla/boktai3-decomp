#include "scene_arc.h"
s32 GroundEffect_Link(struct SceneGroundManager *manager, struct SceneGroundEffect *effect)
{
    u32 active = effect->active;
    struct SceneGroundEffect *head;
    if (active != 0)
        return -1;
    effect->prev = (void *)active;
    head = manager->head;
    effect->next = head;
    if (head != 0)
        head->prev = effect;
    manager->head = effect;
    effect->active = 1;
    return 0;
}
