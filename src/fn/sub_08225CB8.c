#include "global.h"
#include "engine.h"
struct Grid {
    u8 pad0[8];
    s16 originX, originY; /* BG map pixels * 16 */
};
struct TerrainTask {
    u32 unk0;
    struct Grid *grid;
};
struct BgOffset {
    u8 pad0[8];
    s16 x, y;
};
extern struct BgOffset gUnk_030042A0;
void Terrain_LoadHP(s32 hp);
void Terrain_LoadGridResource(u16);
void sub_0821DDC8(u16);
void sub_0821E594(u16);
void sub_0821F15C(u16);
struct TerrainTask *sub_0821A2C4(u32);
void sub_0821447C(s32, void *, void *, void *);
void sub_08217F4C(void);
void sub_08214B44(void);
void sub_08219624(void);
void sub_08218254(void);
void sub_08214D20(void);
void sub_0821964C(void);
/* Script: loads the stage terrain, either from one HP resource (-p) or from
   separate grid (-h), -t, -r and -z resources; -n is read and ignored. -v 0
   installs the default map callbacks and aligns the BG offset with the grid
   origin; another -v value installs the alternative set with offset 0. */
void Terrain_LoadFromScript(void)
{
    s32 grid, t, r, z, hp;
    struct Grid *g;
    Script_GetKeywordValue('n', 0);
    grid = Script_GetKeywordValue('h', 0);
    t = Script_GetKeywordValue('t', 0);
    r = Script_GetKeywordValue('r', 0);
    z = Script_GetKeywordValue('z', 0);
    hp = Script_GetKeywordValue('p', 0);
    if (hp)
        Terrain_LoadHP(hp);
    else {
        if (grid)
            Terrain_LoadGridResource(grid);
        if (t)
            sub_0821DDC8(t);
        if (r)
            sub_0821E594(r);
        if (z)
            sub_0821F15C(z);
    }
    g = sub_0821A2C4(0x56C2)->grid;
    if (Script_SeekToKeyword('v')) {
        s32 mode = Script_GetValue();
        if (mode == 0) {
            sub_0821447C(0, sub_08217F4C, sub_08214B44, sub_08219624);
            gUnk_030042A0.x = g->originX >> 4;
            gUnk_030042A0.y = g->originY >> 4;
        } else {
            sub_0821447C(mode, sub_08218254, sub_08214D20, sub_0821964C);
            gUnk_030042A0.x = 0;
            gUnk_030042A0.y = 0;
        }
    }
}
