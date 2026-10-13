#include "global.h"
#include "scene_object.h"

/* Count how many of the four neighbors accept the object; more than two means it is boxed in. */
u8 sub_0800441C(struct SceneObject *object)
{
    s32 c = 0;
    s32 i = 0;
    do {
        if (Scene_CanEnterNeighborCell(object, object->cellRef.index, i))
            c++;
        i++;
    } while (i <= 3);
    if (c > 2)
        return 1;
    return 0;
}
