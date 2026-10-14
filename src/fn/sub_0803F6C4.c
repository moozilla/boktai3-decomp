#include "scene_arc.h"
void sub_0821A284(u32, void *, u32);
u32 GroundEffect_InitializeManager(struct SceneGroundManager *manager)
{
    sub_0821A284(0xf0de, manager, 0);
    manager->head = 0;
    manager->resource = AnimData_FindEntry(0x1c1a);
    return 0;
}
