#include "global.h"
#include "contact_actor.h"
#include "engine.h"
#include "ram.h"
#include "render.h"

struct FragmentSequence {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unk_0A, speed, elapsed;
};
struct FragmentSprite {
    u32 flags;
    u8 active, unk_05[2], mode;
    u8 unk_08[4];
    void *graphics;
    u8 unk_10[0xC];
    struct ContactPosition position;
    u8 unk_24[8];
};
struct FragmentBurst {
    struct FragmentSprite sprites[4];
    struct FragmentSequence sequences[4];
    struct ContactPosition origin, position;
    u8 collider[0x54];
    u16 age;
    u8 state;
    s8 slot;
    void *completionScript;
    u16 timer;
    u8 enter;
};
struct FragmentBurstPool {
    u8 unk_00[0x18];
    u8 graphics[0x1C];
    void *sequenceResource;
};
struct FragmentBurstPool *FragmentBurst_EnsurePool(void);
struct FragmentBurst *FragmentBurst_AllocateSpriteGroup(struct FragmentBurstPool *);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void sub_081281D0(struct FragmentBurst *, u32, u32, u32, u32);

static inline s32 ProjectScale(s32 input)
{
    s32 value = input * 3;
    if (value >= 0)
        value >>= 5;
    else
        value = -((-value) >> 5);
    return value;
}

static inline u16 GroundHeight(struct ContactPosition *point)
{
    s32 x, z;
    u32 cell;
    u8 *definition;
    u32 type;
    s32 height;
    x = point->x >> 8;
    z = point->z >> 8;
    if (x < 0 || z < 0 || x >= gTerrainWidth || z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    definition = TerrainModifiers_FindCellOverride(cell, 1);
    if (definition)
        definition += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        definition = map->cells + offset;
    }
    type = *definition >> 4;
    height = (*definition & 15) << 8;
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

void FragmentBurst_Create(struct ContactPosition *position, u32 amount, u32 kind, u32 parameter, u32 flags,
                  u32 unused, void *completionScript)
{
    struct ContactPosition screen;
    struct ContactPosition *out;
    struct FragmentBurstPool *pool = gSpriteGroupPool;
    struct FragmentBurst *group;
    s32 i;
    const u16 *randomTable;
    s32 x, z, projected, height;
    if (!pool) {
        pool = FragmentBurst_EnsurePool();
        if (!pool)
            return;
    }
    group = FragmentBurst_AllocateSpriteGroup(pool);
    if (!group)
        return;
    group->state = 1;
    group->age = 0;
    group->timer = 0;
    group->enter = 1;
    group->origin = *position;
    group->completionScript = completionScript;
    group->position = *position;
    group->position.y = GroundHeight(position);
    out = &screen;
    x = position->x;
    z = position->z;
    out->x = ProjectScale(x - z);
    projected = ProjectScale(x + z);
    height = ProjectScale(position->y);
    {
        s32 y = projected - height, depth = projected + height;
        out->x = out->x - (*(struct ContactPosition *)gCameraOrigin).x + 120;
        out->y = y - (*(struct ContactPosition *)gCameraOrigin).y + 80;
        out->z = depth - (*(struct ContactPosition *)gCameraOrigin).z;
    }
    for (i = 0; i < 4; i++) {
        struct FragmentSprite *sprite = &group->sprites[i];
        Sprite_Init(sprite, pool->graphics, 0);
        sprite->flags |= 0x11;
        sprite->mode = 1;
    }
    for (i = 0; i < 3; i++) {
        struct FragmentSprite *sprite = &group->sprites[i];
        struct FragmentSequence *sequence = &group->sequences[i];
        Anim_SetSequence(sequence, pool->sequenceResource, 15, 0, 0);
        gRandomIndex = (gRandomIndex + 1) & 1023;
        randomTable = gRandomTable;
        {
            u32 value = randomTable[gRandomIndex];
            value &= 3;
            sequence->index = value;
        }
        AnimData_SelectPaletteBank(sprite->graphics, 0x13F);
        sprite->position = screen;
        sprite->position.y += i << 5;
    }
    sub_081281D0(group, amount, kind, parameter, flags);
}
