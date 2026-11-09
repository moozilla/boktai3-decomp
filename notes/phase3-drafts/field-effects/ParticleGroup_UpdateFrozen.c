// Pin-free draft of ParticleGroup_UpdateFrozen (src/fn/sub_08014330.c): 3 ROM / 1 draft.
void ParticleGroup_UpdateFrozen(struct PManager *m, struct PGroup *g)
{
    u8 *p;
    u32 base;
    s32 n;
    if (g->type <= 8)
        base = gUnk_08604B90[0x27];
    else
        base = gUnk_08604B90[g->type];
    p = g->particles;
    n = 7;
    do {
        struct PEntry *e;
        u32 cell, v;
        u8 *q;
        u32 fl;
        if (g->type <= 0x14)
            e = (struct PEntry *)m->small;
        else
            e = (struct PEntry *)m->large;
        if (!(*(u32 *)(p + 0x10) & 1)) {
            s32 x = (s16) * (u16 *)(p + 0x28) >> 8;
            s32 y = (s16) * (u16 *)(p + 0x2c) >> 8;
            u32 t;
            if (x < 0 || y < 0 || (u32)x >= gTerrainWidth || (u32)y >= gTerrainHeight)
                t = 0xFFFF;
            else
                t = gTerrainMap->rows[y] + x;
            cell = (u16)t;
            if (cell != 0xFFFF) {
                q = TerrainModifiers_FindCellOverride(cell, 1);
                if (q)
                    q += 4;
                else
                    q = gTerrainMap->cells + 0xc + cell * 4;
                if ((fl = *(u16 *)(q + 2)) & 0x20) {
                    v = 0xFFFF;
                } else {
                    q = TerrainModifiers_FindCellOverride(cell, 1);
                    if (q)
                        q += 4;
                    else
                        q = gTerrainMap->cells + 0xc + cell * 4;
                    v = (q[0] & 0xf) << 8;
                }
            } else
                v = cell;
            if (g->type <= 8 && (m->timer & 1) && v != 0xFFFF) {
                *(u16 *)(p + 0x2a) = v;
                *(u16 *)(p + 0x18) = e->c + base;
            } else {
                *(u16 *)(p + 0x2a) = *(u16 *)(p + 6);
                *(u16 *)(p + 0x18) = e->c + *(u32 *)((g->type << 2) + (u32)gUnk_08604B90);
            }
        }
        p += 0x38;
        n--;
    } while (n >= 0);
}
