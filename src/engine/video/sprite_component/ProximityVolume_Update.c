#include "global.h"
#include "scene/collision/contact_position.h"
#include "engine/video/proximity_volume.h"
extern s16 gUnk_086149C4[];
void NavMesh_UpdateProbe(void *, void *);
u32 ProximityVolume_ContainsPlayer(struct ProximityVolume *);
s32 Math_Atan2(s32, s32);
void AnimatedHazard_CreateProximity(void *, s32, s32, s32);
void SabataFrost_StopEmission(void *);
static inline s32 scaleOffset(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void ProximityVolume_Update(struct ProximityVolume *p)
{
    if (p->active) {
        s32 moved = 0;
        NavMesh_UpdateProbe(p->probe, &p->position);
        if (ProximityVolume_ContainsPlayer(p)) {
            s32 dx = p->targetX - p->position.x;
            s32 dz = p->targetZ - p->position.z;
            if (dx * dx + dz * dz <= 400) {
                p->position.x += dx;
                p->position.z += dz;
                moved = 1;
            } else {
                s32 a = Math_Atan2(dx, dz) & 0xe0;
                p->position.x += scaleOffset(gUnk_086149C4[(a + 0x40) & 0xff] * 8);
                p->position.z += scaleOffset(gUnk_086149C4[a] * 8);
            }
        }
        if (moved == 1) {
            AnimatedHazard_CreateProximity(&p->position, p->damage, 8, 0x1e);
            p->active = 0;
        }
    } else {
        SabataFrost_StopEmission(p->emitter);
    }
}
