#include "global.h"
#include "engine/math/scene_vec.h"

struct GrowthHazard {
    u8 unk0[0x1C];
    struct SceneVec position;
};
void ImpactFragmentGroup_SpawnRadial(u32, u32, struct SceneVec *, u32, u32, u32, u32, u32, u32, u32, u32, u32);

void SolarRoot_PlayImpactEffect(struct GrowthHazard *hazard)
{
    struct SceneVec *p = &hazard->position;
    ImpactFragmentGroup_SpawnRadial(3, 4, p, 0x3c, 0x1e, 0x10, 8, 8, 0, 0x100, 0x18, 0x10);
    ImpactFragmentGroup_SpawnRadial(8, 8, p, 0x3c, 0x1e, 0x16, 8, 8, 0, 0x100, 0x18, 0x10);
}
