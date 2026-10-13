#include "global.h"
#include "scene_object.h"
/* Make every idle type-1 object start disappearing. */
s32 Scene_ActivateTypeOneObjects(void)
{
    struct SceneObjectManager *manager = gUnk_02000024;
    struct SceneObject *p;
    s32 i;
    if (!manager)
        return -1;
    p = manager->objects;
    for (i = 0; i < manager->count; p++, i++) {
        if (!p->active)
            continue;
        if (p->type != 1)
            continue;
        p->health = 0;
        if (!p->kind) {
            p->update = Scene_UpdateObjectDisappearance;
            p->timer = 0;
            p->entered = 1;
            p->kind = 3;
        }
    }
    return 0;
}
