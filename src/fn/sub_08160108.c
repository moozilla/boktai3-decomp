#include "global.h"
#include "engine_globals.h"
#include "ram.h"
#include "player.h"
#ifdef NONMATCHING
// fuzzy: 73.1%
struct BurstPosition {
    u16 x, y, z, pad;
} __attribute__((aligned(4)));
struct BurstData {
    u8 attribute;
    u8 pad1[7];
    struct BurstPosition position;
    u8 unknown10[8];
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
    struct Player *owner;
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
static inline s32 RandomValue(const u16 *table, u32 *index, u32 mask)
{
    u32 offset;
    *index = (*index + 1) & mask;
    offset = *index * 2;
    return *(const u16 *)(offset + (u32)table);
}
static inline u32 RandomByte(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return (u8)table[gRandomIndex];
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
    s32 i;
    const u16 *table = gRandomTable;
    u32 initial = RandomByte(table);
    const u16 *randomTable;
    u32 *index;
    void **sheet;
    u32 mask;
    struct BurstRecord *particle;
    struct BurstData *data;
    u32 phase;
    p->sheet = AnimData_FindEntry(0x1C1E);
    i = 0;
    sheet = &p->sheet;
    randomTable = table;
    phase = 0;
    data = &p->particles[0].data;
    particle = p->particles;
    index = &gRandomIndex;
    mask = 1023;
    for (; i <= 15; i++) {
        const s16 *angles;
        ParticleSprite_Initialize(particle, *sheet, 0);
        Effect_SetOrigin(particle, -4, -4);
        Effect_SetPalette(particle, 1);
        data->angle = (initial + phase) + (RandomValue(randomTable, index, mask) & 7);
        data->period = (RandomValue(randomTable, index, mask) & 3) + 7;
        data->radius = ((RandomValue(randomTable, index, mask) >> 3) & 0x5F) + 0x60;
        data->variant = ((i & 3) << 1) + 1;
        data->speed = (RandomValue(randomTable, index, mask) & 31) + 8;
        {
            s32 radius = data->radius;
            s32 offset;
            angles = gUnk_086149C4;
            offset = Truncate(radius * angles[(data->angle + 64) & 255]);
            data->position.x = p->owner->position.x + offset;
        }
        data->position.y = p->owner->position.y;
        {
            s32 radius = data->radius;
            s32 offset;
            angles = gUnk_086149C4;
            offset = Truncate(radius * angles[data->angle]);
            data->position.z = p->owner->position.z + offset;
        }
        particle->priority = 2;
        data->attribute = 0xEC;
        data->frame = RandomValue(randomTable, index, mask) & 7;
        if (data->frame <= 3)
            SpriteDrawable_SelectTileFrame(particle, *sheet, 4);
        else
            SpriteDrawable_SelectTileFrame(particle, *sheet, 5);
        data->frame = 0;
        data->timer = 0;
        data->lifetime = (RandomValue(randomTable, index, mask) & 15) + 56;
        data->active = 1;
        phase += 16;
        data = (struct BurstData *)((u8 *)data + sizeof(*particle));
        particle++;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08160108);
#endif
