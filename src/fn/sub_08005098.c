#include "global.h"
#include "scene_object.h"

extern const u8 gUnk_086049AC[];

/* Try pushing the neighbor in the direction named by table entry `index`. */
s32 sub_08005098(struct SceneVec *position, u32 index)
{
    return SceneObjects_TryPushNeighbor(position, gUnk_086049AC[index]);
}
