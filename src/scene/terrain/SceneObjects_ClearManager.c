// COMPILER: old_agbcc
#include "global.h"
#include "objects/scene_object.h"
/* SceneObjects_ClearManager: Zeroes the global gSceneObjectManager SceneObjectManager pointer (declared in scene_object.h) without freeing its pool, unlike the task-kill callback sub_08 */

void SceneObjects_ClearManager(void)
{
    gSceneObjectManager = 0;
}
