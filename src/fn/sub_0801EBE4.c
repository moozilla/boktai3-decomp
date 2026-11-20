#include "scene_object_controller.h"
/* SceneObjects_ClearSelection: Zeroes the same offset-0x2e selection field; counterpart to ScriptSetSelection with no script read, matching existing draft name */
#define SceneObjects_ClearSelection sub_0801EBE4
#define SceneObjects_ClearSelection sub_0801EBE4
void SceneObjects_ClearSelection(void)
{
    struct SceneObjPool *p = ResourceRegistry_GetResource(0x85e6);
    if (p)
        p->selection = 0;
}
