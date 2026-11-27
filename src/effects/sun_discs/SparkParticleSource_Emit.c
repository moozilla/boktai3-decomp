#include "global.h"
#include "scene/collision/packed_contact_vector.h"
struct SparkParticleSource {
    u8 unknown[0x94];
    u8 position[8];
};
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void SparkParticleSource_Emit(struct SparkParticleSource *source)
{
    struct PackedContactVector extent;
    extent.x = 160;
    extent.y = 0;
    extent.z = 160;
    SparkParticle_Spawn(source->position, 1, &extent, 0, 0, 20, 1, 2);
}
