#include "global.h"
/* SceneObjects_ScriptSetSelection: Reads a scene-object field and sets its selection-highlight u16 at offset 0x2e from a script-supplied value; existing draft already uses this name */
#define SceneObjects_ScriptSetSelection sub_0801EBBC
#define SceneObjects_ScriptSetSelection sub_0801EBBC
u8 *ResourceRegistry_GetResource(int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
void SceneObjects_ScriptSetSelection(void)
{
    u8 *p = ResourceRegistry_GetResource(0x85e6);
    if (p && Script_SeekToKeyword(0x6e))
        *(u16 *)(p + 0x2e) = Script_GetValue();
}
