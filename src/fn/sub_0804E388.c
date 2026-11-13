#include "area_effect.h"
#include "engine.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void AreaEffect_InitializeEntry(struct AreaEffectEntry *, s32, struct AreaEffectPosition *, s32,
                                u32);
struct PackedPosition {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
static inline s32 AreaEffect_GetCellIndex(struct PackedPosition *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline s32 AreaEffect_GetGroundHeight(struct PackedPosition *position)
{
    s32 index = AreaEffect_GetCellIndex(position), height;
    u8 *cell = TerrainModifiers_FindCellOverride(index, 1);
    u32 kind;
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + 12 + index * 4;
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}
static inline u32 AreaEffect_IsVisible(struct AreaEffectManager *manager)
{
    return manager->visible;
}
void AreaEffect_AddSpriteFromScript(void)
{
    struct AreaEffectManager *manager = gAreaEffectManager;
    if (manager == NULL || manager->count > 15)
        return;
    {
        struct PackedPosition position;
        u32 kind, rotated;
        if (Script_SeekToKeyword(0x6E))
            manager->entries[manager->count].tag = Script_GetValue();
        else
            manager->entries[manager->count].tag = 0;
        if (Script_SeekToKeyword(0x70)) {
            position.x = Script_GetValue();
            position.y = Script_GetValue();
            position.z = Script_GetValue();
        } else {
            position.x = 0;
            position.y = 0;
            position.z = 0;
        }
        position.y = AreaEffect_GetGroundHeight(&position);
        if (Script_SeekToKeyword(0x74))
            kind = Script_GetValue();
        else
            kind = 0;
        if (Script_SeekToKeyword(0x72))
            rotated = Script_GetValue();
        else
            rotated = 0;
        AreaEffect_InitializeEntry(&manager->entries[manager->count], kind,
                                   (struct AreaEffectPosition *)&position, rotated,
                                   manager->graphicsId);
        manager->entries[manager->count].enabled = 1;
        if (!AreaEffect_IsVisible(manager))
            manager->entries[manager->count].spriteFlags |= 1;
        manager->count++;
    }
}
