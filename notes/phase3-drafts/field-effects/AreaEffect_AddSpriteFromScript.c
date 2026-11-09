/* AreaEffect_AddSpriteFromScript (src/fn/sub_0804E388.c): plain draft, no concessions.
 * 19 ROM / 19 draft insns differ, all register choice: global alloc runs manager (priority
 * 1604: 10 refs, live 187) before the 0xA8 stride constant (1600, REG_EQUIV doubled life), so
 * manager takes r5 instead of r6 (AGBCC_PRI=<stride>:+5 matches). Needs manager live 188+.
 * The groundHeight/cellIndex inlines (as in src/item_container.c) give the ROM's point copy
 * without the barrier; gcse copies &manager->count into r8 without the pin.
 * Tried without effect on the live length: ternaries, getCell inline, prototype types,
 * early-return forms, count pointer forms, cellIndex forms. */
struct PackedPosition {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
static inline s32 cellIndex(struct PackedPosition *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline s32 groundHeight(struct PackedPosition *position)
{
    s32 index = cellIndex(position), height;
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
void AreaEffect_AddSpriteFromScript(void)
{
    struct AreaEffectManager *manager = gAreaEffectManager;
    if (manager == NULL || manager->count > 15)
        return;
    {
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
            position.y = groundHeight(&position);
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
            if (!manager->visible)
                manager->entries[manager->count].spriteFlags |= 1;
            manager->count++;
        }
    }
}
