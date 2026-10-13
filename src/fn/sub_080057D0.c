#include "global.h"
#include "scene_object.h"

/* Find a free object slot, or NULL. */
struct SceneObject *sub_080057D0(struct SceneObjectManager *manager)
{
    struct SceneObject *e = manager->objects;
    s32 i;
    for (i = 0; i < manager->count; e++, i++) {
        if (e->active == 0)
            return e;
    }
    return 0;
}
