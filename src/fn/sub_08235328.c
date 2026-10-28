#include "global.h"
#include "render.h"
#include "terrain_cell_probe.h"

struct BurstSprite {
    u32 flags;
    u8 unk_04[0x18];
    struct Pos16 position;
    u8 unk_24[8];
};

struct HealthBurstEffect {
    u8 unk_00[0xe8];
    struct BurstSprite sprites[2];
    u8 unk_140[0x1c];
    u16 nextSprite;
    u8 active[2];
    u16 floorHeight[2];
    u16 timer[2];
    u8 unk_168[0x1e6];
    u16 age;
};

s32 Div(s32, s32);

static inline u32 BurstRandom(void)
{
    const u16 *table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

static inline s32 ScaleTowardZero(s32 value, s32 shift)
{
    if (value >= 0)
        value >>= shift;
    else
        value = -((-value) >> shift);
    return value;
}

static inline s32 ScaleTowardZeroResult(s32 value, s32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -((-value) >> shift);
}

static inline u32 BurstCellFlags(struct Pos16 *point)
{
    u8 *props = cellProps(cellIndexAt(point));
    u16 *field = (u16 *)(props + 2);
    u32 value = *field;
    return value;
}

static inline s32 BurstFloorHeight(struct Pos16 *point)
{
    u8 *props = cellProps(cellIndexAt(point));
    u32 type = *props >> 4;
    s32 height = (*props & 15) << 8;
    switch (type) {
        case 1:
            height -= (u8)point->z;
            break;
        case 2:
            height -= (u8)point->x;
            break;
    }
    return height;
}

void HealthBurstEffect_StartBurst(struct HealthBurstEffect *effect)
{
    struct BurstSprite *sprite = &effect->sprites[effect->nextSprite];
    s32 sector, angle, radius, x, z;
    s32 coordinateX, coordinateZ;

    sector = Div(effect->age, 18);
    angle = (sector << 6) + (BurstRandom() & 63);
    angle &= 255;
    radius = (BurstRandom() & 255) + 128;
    x = ScaleTowardZero(gUnk_086149C4[(angle + 64) & 255] * radius, 12);
    z = ScaleTowardZeroResult(radius * gUnk_086149C4[angle] * 3, 13);
    sprite->flags &= ~1;
    sprite->position = *(struct Pos16 *)&gCamera.position;
    coordinateX = sprite->position.x - 75;
    sprite->position.x = coordinateX + (x - z);
    sprite->position.y += 850;
    coordinateZ = sprite->position.z - 75;
    sprite->position.z = coordinateZ + (x + z);
    effect->active[effect->nextSprite] = 1;
    effect->timer[effect->nextSprite] = 0;
    if (BurstCellFlags(&sprite->position) & 0x40) {
        effect->floorHeight[effect->nextSprite] = 0;
    } else {
        effect->floorHeight[effect->nextSprite] = BurstFloorHeight(&sprite->position);
    }
    effect->nextSprite = 1 - effect->nextSprite;
}
