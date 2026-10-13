#include "global.h"
#include "scene_object.h"

void sub_08219D38(u32);

/* Task teardown: free all objects, release the pool and clear the global manager. */
u32 sub_080056D0(struct SceneObjectManager *manager)
{
    sub_080047EC();
    sub_08219D38((u32)manager->objects);
    return (u32)(gUnk_02000024 = 0);
}
