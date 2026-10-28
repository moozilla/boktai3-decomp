#include "global.h"
#include "ram.h"
#ifdef NONMATCHING
// fuzzy: 57.0%
// fix.py best (sweep after the inlined-insn fix)
struct BurstPosition {
    u16 x, y, z, pad;
} __attribute__((aligned(4)));
struct BurstOwner {
    u8 pad0[0x30];
    struct BurstPosition position;
};
struct BurstData {
    u8 attribute;
    u8 pad1[7];
    struct BurstPosition position;
    u8 active, angle, variant, speed, frame, period, lifetime, timer;
    u16 radius;
    u8 pad22[0xA];
};
struct BurstRecord {
    u8 pad0[0xF], priority;
    struct BurstData data;
};
struct PlayerBurstTask {
    u8 pad0[0x18];
    struct BurstOwner *owner;
    u8 pad1C[0xDC];
    void *sheet;
    struct BurstRecord particles[16];
};
extern const s16 gUnk_086149C4[];
void *AnimData_FindEntry(u32);
void ParticleSprite_Initialize(void *, void *, s32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
static inline s32 RandomValue(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}
static inline s32 Truncate(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
// confidence=low: initializes sixteen radial particles around the attached player.
#define PlayerBurst_InitializeParticles sub_08160108
void PlayerBurst_InitializeParticles(struct PlayerBurstTask *p)
{
    const u16 *table = (const u16 *)0x0203B400;
    u32 initial = RandomValue(table) & 255;
    struct BurstRecord *particle;
    struct BurstData *data;
    s32 i;
    u32 phase = 0;
    typeof(i++) t1;
    p->sheet = AnimData_FindEntry(0x1C1E);
    particle = p->particles;
    data = &particle->data;
    t1 = i++;
    for (i = 0; i <= 15; t1, phase += 16, particle++, data = (void *)(((u8 *)data) + 0x3C)) {
        ParticleSprite_Initialize(particle, p->sheet, 0);
        Effect_SetOrigin(particle, -4, -4);
        Effect_SetPalette(particle, 1);
        data->angle = (initial + phase) + (RandomValue(table) & 7);
        data->period = (RandomValue(table) & 3) + 7;
        data->radius = ((RandomValue(table) >> 3) & 0x5F) + 0x60;
        data->variant = ((i & 3) << 1) + 1;
        data->speed = (RandomValue(table) & 31) + 8;
        {
            s32 offset = Truncate(gUnk_086149C4[(data->angle + 64) & 255] * data->radius);
            data->position.x = p->owner->position.x + offset;
        }
        data->position.y = p->owner->position.y;
        data->position.z =
            p->owner->position.z + Truncate(data->radius * gUnk_086149C4[data->angle]);
        particle->priority = 2;
        data->attribute = 0xEC;
        data->frame = RandomValue(table) & 7;
        if (data->frame <= 3)
            SpriteDrawable_SelectTileFrame(particle, p->sheet, 4);
        else
            SpriteDrawable_SelectTileFrame(particle, p->sheet, 5);
        data->frame = 0;
        data->timer = 0;
        data->lifetime = (RandomValue(table) & 15) + 56;
        data->active = 1;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08160108);
#endif
