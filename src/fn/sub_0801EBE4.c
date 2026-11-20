#include "scene_object_controller.h"
void SceneObjects_ClearSelection(void)
{
    struct SceneObjPool *p = ResourceRegistry_GetResource(0x85e6);
    if (p)
        p->selection = 0;
}
