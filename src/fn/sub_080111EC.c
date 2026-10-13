#include "global.h"
struct Menu {
    u8 pad[0x450];
    void *map, *palette;
};
void *ScriptProc_Start(int, int);
void sub_082161B4(int, int, void *, int, int, int, void *);
void BgGfx_LoadTilemapSimple(int, int, int, int, int);
extern u16 gUnk_03004FC0[];
void MessageMenu_LoadBackground(struct Menu *m, int id)
{
    void *map = ScriptProc_Start(0xC091, 0x3536);
    m->map = map;
    sub_082161B4(1, 0, map, 0, 0, 1, &id);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    m->palette = (u8 *)ScriptProc_Start(0x92B3, 0x204) + 20;
    CpuSet(m->palette, gUnk_03004FC0, 256);
}
