#include "global.h"
u8 *ScriptProc_Start(int, int);
void sub_082161B4(int, int, void *, int, int, int, void *);
void BgGfx_LoadTilemapSimple(int, int, int, int, int);
extern u8 gUnk_03005160[];
int SelectionMenu_ConfigureLayers(int value, int mode)
{
    u8 *resource = ScriptProc_Start(0xc091, 0xa597);
    int result;
    if (mode) {
        sub_082161B4(3, 0, resource, 0, 0, 1, &value);
        BgGfx_LoadTilemapSimple(1, 0, 0, 0, 0);
        result = 1;
    } else {
        sub_082161B4(0, 0, resource, 0, 0, 1, &value);
        BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
        result = 0;
    }
    {
        u8 *palette = ScriptProc_Start(0x92b3, 0x81b);
        CpuSet(palette + 0x1b4, gUnk_03005160, 0x30);
    }
    return result;
}
