#include "global.h"
/* SceneObjects_ClearSelection: Zeroes the same offset-0x2e selection field; counterpart to ScriptSetSelection with no script read, matching existing draft name */
#define SceneObjects_ClearSelection sub_0801EBE4
#define SceneObjects_ClearSelection sub_0801EBE4
u8 *ResourceRegistry_GetResource(int);
void SceneObjects_ClearSelection(void)
{
    u8 *p = ResourceRegistry_GetResource(0x85e6);
    if (p)
        *(u16 *)(p + 0x2e) = 0;
}
