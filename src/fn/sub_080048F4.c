#include "global.h"
#include "scene_object.h"
u32 sub_0821ABA8(u32, u32);
/* Free every active object whose script id matches the selected one. */
s32 sub_080048F4(void)
{
    struct SceneObjectManager *manager = gUnk_02000024;
    u32 id;
    struct SceneObject *object;
    s32 i;
    if (manager == 0)
        return -1;
    id = sub_0821ABA8(0x6e, 0);
    object = manager->objects;
    for (i = 0; i < manager->count; object++, i++) {
        if (object->active != 0 && object->body.id == id)
            sub_080047AC(object);
    }
    return 0;
}
