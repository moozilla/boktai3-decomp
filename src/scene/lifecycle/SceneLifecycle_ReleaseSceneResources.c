#include "global.h"

void ResourceRegistry_ClearEntries(u32);
void ResourceSlots_Reset(void);

void SceneLifecycle_ReleaseSceneResources(void)
{
    ResourceRegistry_ClearEntries(0);
    ResourceSlots_Reset();
}
