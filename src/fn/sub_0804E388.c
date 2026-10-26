#include "area_effect.h"
#include "engine.h"
// MATCHING_CONCESSION: register-pinning
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void AreaEffect_InitializeEntry(struct AreaEffectEntry *, s32, struct AreaEffectPosition *, s32,
                                u32);
struct PackedPosition {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
static inline s32 cellIndex(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
void AreaEffect_AddSpriteFromScript(void)
{
    struct AreaEffectManager *manager = gAreaEffectManager;
    if (manager) {
        u8 *cursor = &manager->count;
        u32 n = *cursor;
        register u8 *count asm("r8") = cursor;
        if (n <= 15) {
            struct PackedPosition position;
            register struct PackedPosition *point asm("r5");
            u32 found = Script_SeekToKeyword(0x6E);
            u32 kind, rotated;
            s32 cell;
            u8 *properties;
            u32 surface, height;
            if (found)
                manager->entries[*count].tag = Script_GetValue();
            else
                manager->entries[*count].tag = found;
            if (Script_SeekToKeyword(0x70)) {
                position.x = Script_GetValue();
                position.y = Script_GetValue();
                position.z = Script_GetValue();
            } else {
                u32 mask = 0xFFFF0000;
                ((u32 *)&position)[0] = 0;
                ((u32 *)&position)[1] &= mask;
            }
            point = &position;
            {
                struct PackedPosition *first = point;
                asm("" : "+r"(first));
                cell = cellIndex(first->x >> 8, point->z >> 8);
            }
            properties = TerrainModifiers_FindCellOverride(cell, 1);
            if (properties)
                properties += 4;
            else {
                struct EngineMap *map = gTerrainMap;
                u32 offset = cell * 4 + 12;
                properties = map->cells + offset;
            }
            surface = *properties >> 4;
            height = (*properties & 15) << 8;
            switch (surface) {
                case 1:
                    height -= ((u8 *)point)[4];
                    break;
                case 2:
                    height -= ((u8 *)point)[0];
                    break;
            }
            ((struct AreaEffectPosition *)&position)->y = height;
            if (Script_SeekToKeyword(0x74))
                kind = Script_GetValue();
            else
                kind = 0;
            if (Script_SeekToKeyword(0x72))
                rotated = Script_GetValue();
            else
                rotated = 0;
            {
                u8 *entryCount = count;
                AreaEffect_InitializeEntry(&manager->entries[*entryCount], kind,
                                           (struct AreaEffectPosition *)&position, rotated,
                                           manager->graphicsId);
                {
                    u32 enabledOffset = *entryCount * sizeof(struct AreaEffectEntry);
                    u8 *enabled = (u8 *)manager + enabledOffset + 0xC6;
                    u32 one = 1;
                    *enabled = one;
                    if (!manager->visible) {
                        u32 offset = *entryCount * sizeof(struct AreaEffectEntry);
                        u8 *base = (u8 *)manager + 0x34;
                        u32 *flags;
                        flags = (u32 *)(base + offset);
                        *flags |= one;
                    }
                }
            }
            {
                u8 *destination = count;
                u32 value = *destination;
                *destination = value + 1;
            }
        }
    }
}
