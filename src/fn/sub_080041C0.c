// COMPILER: old_agbcc
#include "global.h"
/* SceneObjects_ClearManager: Zeroes the global gUnk_02000024 SceneObjectManager pointer (declared in scene_object.h) without freeing its pool, unlike the task-kill callback sub_08 */
#define SceneObjects_ClearManager sub_080041C0

extern u32 gUnk_02000024;

void sub_080041C0(void)
{
    gUnk_02000024 = 0;
}
