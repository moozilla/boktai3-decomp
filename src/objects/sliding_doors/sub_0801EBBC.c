#include "objects/sliding_doors/scene_object_controller.h"
void SlidingDoors_ScriptSetSelection(void)
{
    struct SceneObjPool *p = ResourceRegistry_GetResource(0x85e6);
    if (p && Script_SeekToKeyword(0x6e))
        p->selection = Script_GetValue();
}
