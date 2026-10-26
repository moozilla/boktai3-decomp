#include "global.h"
#include "engine.h"
// MATCHING_CONCESSION: register-pinning
struct PGroup {
    u8 active, count, type, priority;
    u8 particles[448];
};
struct PManager {
    u8 pad[24];
    u32 timer;
    u8 *small, *large;
    struct PGroup groups[6];
};
struct PCell {
    u8 b0, b1;
    u16 flags;
};
struct PMap {
    u32 pad0;
    u8 *cells;
    u8 pad1[28];
    u16 rows[1];
};
struct PEntry {
    u16 a, b;
    u16 c, d;
};
extern u32 gUnk_08604B90[];
u8 *TerrainModifiers_FindCellOverride(s32, s32);

void ParticleGroup_UpdateFrozen(struct PManager *m, struct PGroup *g)
{
    u8 *p;
    u32 base;
    s32 n;
    u32 type;
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
        type = ({
            register u32 z asm("r0") = g->type;
            asm("" : "+r"(z));
            z;
        });

        if (type <= 0x14)
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
                    type = g->type;
                } else {
                    q = TerrainModifiers_FindCellOverride(cell, 1);
                    if (q)
                        q += 4;
                    else
                        q = gTerrainMap->cells + 0xc + cell * 4;
                    v = (q[0] & 0xf) << 8;
                    type = g->type;
                }
            } else
                v = cell;
            if (({ (u8) type; }) <= 8 && (m->timer & 1) && v != 0xFFFF) {
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
