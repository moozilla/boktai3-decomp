#include "global.h"
struct SparkParticleExtent {
    s32 x : 16, y : 16, z : 16, padding : 16;
};
struct SparkParticleSource {
    u8 unknown[0x94];
    u8 position[8];
};
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void SparkParticleSource_Emit(struct SparkParticleSource *source)
{
    struct SparkParticleExtent extent;
    extent.x = 160;
    extent.y = 0;
    extent.z = 160;
    SparkParticle_Spawn(source->position, 1, &extent, 0, 0, 20, 1, 2);
}
