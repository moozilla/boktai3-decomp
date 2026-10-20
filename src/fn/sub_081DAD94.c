#include "global.h"

#ifdef NONMATCHING
// fuzzy: 95.2%
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
    register s32 dir asm("r4");
    s32 speed;
    s16 *v;
    s32 angle;
    if (!task || index < 0 || index >= task->count)
        return 1;
    entry = &task->entries[index];
    params = &entry->params;
    dir = (s8)direction;
    Projectile_SetFlightParams(params, entry->params.a, params->b, dir, params->d);
    entry->direction = dir;
    entry->sector = ((u16)dir + 0x70 >> 5) & 7;
    v = &entry->vx;
    angle = params->c;
    speed = params->d;
    v[0] = scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    v[1] = 0;
    v[2] = scale(gUnk_086149C4[angle & 255] * speed);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081DAD94);
#endif
