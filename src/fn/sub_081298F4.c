#define SparkParticleSource_Emit sub_081298F4
#define SparkParticle_Spawn sub_0806A160
#include "global.h"
struct SparkParticleExtent {
    s32 x : 16, y : 16, z : 16, padding : 16;
};
struct SparkParticleSource {
    u8 unknown[0x94];
    u8 position[8];
};
void sub_0806A160(void *, s32, void *, s32, s32, s32, s32, s32);
void sub_081298F4(struct SparkParticleSource *source)
{
    struct SparkParticleExtent extent;
    extent.x = 160;
    extent.y = 0;
    extent.z = 160;
    sub_0806A160(source->position, 1, &extent, 0, 0, 20, 1, 2);
}
