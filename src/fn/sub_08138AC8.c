#include "global.h"
#include "scene_pool.h"
struct HeightProbe {
    u8 unknown00[0x9c];
    struct ScenePoolVec32 position;
    u8 unknownAC[0x20];
    s32 heightOffset;
};
s32 Terrain_GetHeightAtFixedPosition(struct ScenePoolVec32 *);
u8 HeightProbe_IsBelowLimit(struct HeightProbe *p)
{
    struct ScenePoolVec32 *position = &p->position;
    s32 height = Terrain_GetHeightAtFixedPosition(position);
    s32 offset = p->heightOffset;
    s32 y = position->y;
    s32 limit = position->z + (y + offset);
    if (height < (limit >> 12) + 0x70) return 1;
    else return 0;
}
