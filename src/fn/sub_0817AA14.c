#include "actor_spatial_query.h"
// Tentative SceneVolume family: ownership and field semantics remain provisional.
#define PositionSearch_GetCoordinates SceneStimulus_GetTaggedPosition
#define PositionSearch_FindNearby     SceneStimulus_FindOverlap
u32 SceneStimulus_GetTaggedPosition(u16 *, u32);
s8 SceneStimulus_FindOverlap(u32, u32, u32, u8, u16);
s32 SceneVolume_FindNearbyPosition(struct SceneVolumeSearch *volume)
{
    s8 index = SceneStimulus_FindOverlap(volume->x, volume->z, 0x400, 1, volume->searchFlags);
    s32 result;
    if (index >= 0) {
        SceneStimulus_GetTaggedPosition(volume->coordinates, index);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}
