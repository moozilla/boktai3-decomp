#include "scene_arc.h"
void ResourceRegistry_RegisterResource(u32, void *, u32);
u32 GroundEffect_InitializeManager(struct SceneGroundManager *manager)
{
    ResourceRegistry_RegisterResource(0xf0de, manager, 0);
    manager->head = 0;
    manager->resource = AnimData_FindEntry(0x1c1a);
    return 0;
}
