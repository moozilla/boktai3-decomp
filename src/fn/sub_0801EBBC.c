#include "scene_object_controller.h"
/* SceneObjects_ScriptSetSelection: Reads a scene-object field and sets its selection-highlight u16 at offset 0x2e from a script-supplied value; existing draft already uses this name */
#define SceneObjects_ScriptSetSelection sub_0801EBBC
#define SceneObjects_ScriptSetSelection sub_0801EBBC
void SceneObjects_ScriptSetSelection(void)
{
    struct SceneObjPool *p = ResourceRegistry_GetResource(0x85e6);
    if (p && Script_SeekToKeyword(0x6e))
        p->selection = Script_GetValue();
}
