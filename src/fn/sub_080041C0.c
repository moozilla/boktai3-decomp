// COMPILER: old_agbcc
#include "global.h"
#include "scene_object.h"
/* SceneObjects_ClearManager: Zeroes the global gSceneObjectManager SceneObjectManager pointer (declared in scene_object.h) without freeing its pool, unlike the task-kill callback sub_08 */
#define SceneObjects_ClearManager sub_080041C0

void sub_080041C0(void)
{
    gSceneObjectManager = 0;
}
