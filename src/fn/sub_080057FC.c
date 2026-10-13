#include "global.h"
#include "scene_object.h"

/* Find the active object with the given script id, or NULL. */
struct SceneObject *sub_080057FC(struct SceneObjectManager *manager, u32 id)
{
    struct SceneObject *e = manager->objects;
    s32 i;
    for (i = 0; i < manager->count; e++, i++) {
        if (e->active != 0 && e->body.id == id)
            return e;
    }
    return 0;
}
