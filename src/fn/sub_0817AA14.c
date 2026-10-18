#include "global.h"
// Tentative SceneVolume family: ownership and field semantics remain provisional.
#define PositionSearch_GetCoordinates SceneStimulus_GetTaggedPosition
#define PositionSearch_FindNearby     SceneStimulus_FindOverlap
struct SceneVolume {
    u8 pad0[0x48];
    u32 searchFlags;
    u8 pad4C[8];
    s16 x, y, z;
    u8 pad5A[0x426];
    u16 coordinates[3];
};
u32 SceneStimulus_GetTaggedPosition(u16 *, u32);
s8 SceneStimulus_FindOverlap(u32, u32, u32, u8, u16);
s32 SceneVolume_FindNearbyPosition(struct SceneVolume *volume)
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
