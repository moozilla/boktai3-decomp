// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct ProximityVolume {
    u8 unknown0[8];
    u16 x;
    u16 y;
    u16 z;
    u8 unknown0E[2];
    void *emitter;
    u8 probe[0x1c];
    s16 targetX;
    u8 unknown32[2];
    s16 targetZ;
    u8 unknown36[2];
    s16 active;
    s16 damage;
};
extern s16 gUnk_086149C4[];
void NavMesh_UpdateProbe(void *, void *);
u32 ProximityVolume_ContainsPlayer(struct ProximityVolume *);
s32 Math_Atan2(s32, s32);
void AnimatedHazard_CreateProximity(void *, s32, s32, s32);
void FrostEmitter_StopEmission(void *);
static inline s32 scaleOffset(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
#define ProximityVolume_Update sub_08180730
void ProximityVolume_Update(struct ProximityVolume *p0)
{
    register struct ProximityVolume *p asm("r5") = p0;
    s32 moved;
    u16 *pos;
    if (p->active) {
        moved = 0;
        NavMesh_UpdateProbe(p->probe, pos = &p->x);
        if (ProximityVolume_ContainsPlayer(p)) {
            s32 dx = p->targetX - (s16)p->x;
            s32 dz = p->targetZ - (s16)p->z;
            if (dx * dx + dz * dz <= 400) {
                p->x += dx;
                p->z += dz;
                moved = 1;
            } else {
                s32 a = Math_Atan2(dx, dz) & 0xe0;
                p->x += scaleOffset(gUnk_086149C4[(a + 0x40) & 0xff] * 8);
                p->z += scaleOffset(gUnk_086149C4[a] * 8);
            }
        }
        if (moved == 1) {
            AnimatedHazard_CreateProximity(pos, p->damage, 8, 0x1e);
            p->active = 0;
        }
    } else {
        FrostEmitter_StopEmission(p->emitter);
    }
}
