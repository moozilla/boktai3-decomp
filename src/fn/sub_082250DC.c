#include "global.h"

void ResourceRegistry_ClearEntries(u32);
void sub_08217298(void);

void SceneLifecycle_ReleaseSceneResources(void)
{
    ResourceRegistry_ClearEntries(0);
    sub_08217298();
}
