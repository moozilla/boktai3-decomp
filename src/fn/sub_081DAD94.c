#include "global.h"

extern const s16 gUnk_086149C4[];
void *ResourceRegistry_GetResource(u32);
void Projectile_SetFlightParams(void *, u16, u16, s32, u32);
struct Flight {
    u16 a, b;
    u8 pad4[2];
    s8 c;
    u8 pad7;
    u32 d;
};
struct Entry4E01 {
    u8 pad0[0x10];
    s16 vx, vy, vz;
    u8 pad16[2];
    struct Flight params;
    u8 pad28[0xFA - 0x18 - sizeof(struct Flight)];
    u8 direction;
    u8 padFB[0x17C - 0xFB];
    u16 sector;
    u8 pad17E[0x1AC - 0x17E];
};
struct Task4E01 {
    u8 pad0[0x3C];
    s32 count;
    u8 pad40[0x108 - 0x40];
    struct Entry4E01 *entries;
};
static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
s32 sub_081DAD94(u8 direction, s32 index)
{
    struct Task4E01 *task = ResourceRegistry_GetResource(0x4E01);
    struct Entry4E01 *entry;
    struct Flight *params;
    s32 dir;
    s32 speed;
    s16 *v;
    s32 angle;
    if (!task || index < 0 || index >= task->count)
        return 1;
    entry = &task->entries[index];
    params = &entry->params;
    {
        u16 pa = entry->params.a;
        u16 pb = params->b;
        dir = (s8)direction;
        Projectile_SetFlightParams(params, pa, pb, dir, params->d);
    }
    entry->direction = dir;
    entry->sector = ((u16)dir + 0x70 >> 5) & 7;
    v = &entry->vx;
    angle = params->c;
    speed = params->d;
    {
        s32 sv;
        u32 zero;
        sv = scale(gUnk_086149C4[(angle + 64) & 255] * speed);
        zero = 0;
        v[0] = sv;
        v[1] = zero;
    }
    v[2] = scale(gUnk_086149C4[angle & 255] * speed);
    return 0;
}
