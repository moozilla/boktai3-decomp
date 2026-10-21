#include "global.h"
struct MapIconEntry {
    u8 x, y, big, flag;
};
extern const struct MapIconEntry gUnk_08611B80[];
u32 GameState_GetWorldMapFlag(u32);
u16 *Hud_GetTilemapCell(u32, s32, s32);
// MATCHING_CONCESSION: register-pinning
void MapMenu_DrawLocationIcons(void *menu, s32 left, s32 top)
{
    s32 i = 0, j, k;
    u16 *d;
    s32 base;
    s32 right = left + 32;
    s32 bottom = top + 32;
    for (i = 0; i <= 9; i++) {
        const struct MapIconEntry *tbl = gUnk_08611B80;
        const struct MapIconEntry *e;
        asm("" : "+r"(tbl));
        e = (const struct MapIconEntry *)((i << 2) + (u32)tbl);
        if (GameState_GetWorldMapFlag(e->flag)) {
            s32 x = e->x;
            s32 y = e->y;
            s32 x1;
            base = e->big ? 12 : 8;
            if (x >= left && x < right && y >= top && y < bottom) {
                d = Hud_GetTilemapCell(1, x, y);
                *d = base | 0xE000;
            }
            x1 = x + 1;
            if (x1 >= left && x1 < right && y >= top && y < bottom) {
                d = Hud_GetTilemapCell(1, x1, y);
                *d = (base + 1) | 0xE000;
            }
            if (x >= left && x < right && y + 1 >= top && y + 1 < bottom) {
                d = Hud_GetTilemapCell(1, x, y + 1);
                *d = (base + 16) | 0xE000;
            }
            if (x1 >= left && x1 < right && y + 1 >= top && y + 1 < bottom) {
                d = Hud_GetTilemapCell(1, x1, y + 1);
                *d = (base + 17) | 0xE000;
            }
        }
    }
    if (!GameState_GetWorldMapFlag(14)) {

        k = 0;
        for (i = 0; i <= 5; i++) {
            for (j = 0; j <= 5; j++) {
                if (i + j <= 1)
                    continue;
                if (i + j > 8)
                    continue;
                if (i - j > 3)
                    continue;
                {
                    s32 t = k + 17;
                    if (k <= 1)
                        t = k + 3;
                    *Hud_GetTilemapCell(1, j + 43, i + 15) = t | 0xE000;
                    k++;
                    if (k > 3)
                        k = 0;
                }
            }
        }
    }
}
