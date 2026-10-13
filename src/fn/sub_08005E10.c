#include "global.h"
#define SceneController_ClearGlobal sub_08005E10
extern void *gUnk_02000028;
void SceneController_ClearGlobal(void)
{
    gUnk_02000028 = 0;
}
