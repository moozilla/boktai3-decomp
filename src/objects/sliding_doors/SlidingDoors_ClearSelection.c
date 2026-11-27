#include "objects/sliding_doors/scene_object_controller.h"
void SlidingDoors_ClearSelection(void)
{
    struct SceneObjPool *p = ResourceRegistry_GetResource(0x85e6);
    if (p)
        p->selection = 0;
}
