#include "global.h"
#include "contact_actor.h"
void CpuSet(const void *, void *, u32);
extern const s16 gUnk_086149C4[];
static inline s32 Scale(s32 x)
{
    s32 result;
    if (x >= 0)
        result = x >> 12;
    else
        result = -(-x >> 12);
    return result;
}
inline static void CpuSet_2(int a2, struct ContactPosition *a1, u32 *a0)
{
    CpuSet(a0, a1, a2);
}
void Axe_UpdateTrailColliders(u8 *actor)
{
    u8 *ctx = *(u8 **)(actor + 0x3d0);
    u8 *entry;
    u32 idx;
    s32 i;
    struct {
        u32 zero;
        u32 values[0];
    } buf;
    struct ContactPosition offset;
    union {
        struct ContactPosition p;
        struct ContactPackedVector v;
    } position;
    s32 x, z;
    u16 *ip = (u16 *)(ctx + 0xe44);
    if (*ip == 0)
        idx = 3;
    else
        idx = *ip - 1;
    entry = ctx + (idx * 0x60 + 0x708);
    for (i = 0; i < 3; i++) {
        buf.zero = 0;
        CpuSet_2(0x05000002, &offset, &buf.zero);
        position.p = *(struct ContactPosition *)(ctx + 0xa24);
        switch (i) {
            case 0:
                offset.x = -120;
                offset.z = -120;
                break;
            case 1:
                offset.x = -30;
                offset.z = -30;
                break;
            case 2:
                offset.x = 60;
                offset.z = 60;
                break;
        }
        x = Scale(offset.x * gUnk_086149C4[(entry[6] + 64) & 255]);
        z = Scale(offset.z * gUnk_086149C4[entry[6]]);
        position.v.x = x + z + position.p.x;
        x = Scale(-(offset.x * gUnk_086149C4[entry[6]]));
        z = Scale(offset.z * gUnk_086149C4[(entry[6] + 64) & 255]);
        position.v.z = x + z + position.p.z;
        Collider_SetPosition(ctx + 0x928 + i * 0x54, &position, 0);
        Collider_Refresh(ctx + 0x928 + i * 0x54);
    }
}
