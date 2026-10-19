#include "global.h"
#define SceneObject_ClearEmbeddedFlag sub_081C61AC
struct SceneObjectFlagBlock { u32 unknown0[2]; u32 flags; };
struct SceneObjectFlagOwner { u8 unknown0[0xb8]; struct SceneObjectFlagBlock block; };
struct SceneObjectFlagOwner *sub_081C61AC(struct SceneObjectFlagOwner *p)
{
    struct SceneObjectFlagBlock *block = &p->block;
    block->flags &= ~1;
    return p;
}
