#include "global.h"
#include "scene_object.h"

/* Free every active object. */
s32 sub_080047EC(void)
{
    struct SceneObjectManager *manager = gUnk_02000024;
    struct SceneObject *object;
    s32 i;
    if (manager == 0)
        return -1;
    object = manager->objects;
    for (i = 0; i < manager->count; i++) {
        if (object->active != 0)
            sub_080047AC(object);
        object = object + 1;
    }
    return 0;
}
