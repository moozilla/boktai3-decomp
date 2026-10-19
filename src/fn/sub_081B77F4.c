#include "global.h"
#include "scene_vec.h"

#define GrowthHazard_PlayImpactEffect sub_081B77F4

struct GrowthHazard {
    u8 unk0[0x1C];
    struct SceneVec position;
};
void sub_08014CA4(u32, u32, struct SceneVec *, u32, u32, u32, u32, u32, u32, u32, u32, u32);

void sub_081B77F4(struct GrowthHazard *hazard)
{
    struct SceneVec *p = &hazard->position;
    sub_08014CA4(3, 4, p, 0x3c, 0x1e, 0x10, 8, 8, 0, 0x100, 0x18, 0x10);
    sub_08014CA4(8, 8, p, 0x3c, 0x1e, 0x16, 8, 8, 0, 0x100, 0x18, 0x10);
}
