#include "global.h"
struct SceneDrawablePair {
    u8 unknown0[0x24];
    u32 flags;
    u8 unknown28[0x9E];
    u16 firstFlags;
    u8 unknownC8[0x52];
    u16 secondFlags;
};
s32 SceneDrawablePair_SetFlags(struct SceneDrawablePair *p)
{
    p->firstFlags |= 4;
    p->secondFlags |= 4;
    p->flags |= 1;
    return 0;
}
