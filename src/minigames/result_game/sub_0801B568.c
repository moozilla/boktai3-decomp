#include "global.h"
void *ScriptProc_Start(int, int);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void BgGfx_LoadTilemapSimple(int, int, int, int, int);
int ResultMenu_InitializeLayer(void)
{
    void *resource = ScriptProc_Start(0xc091, 0xa597);
    int value = 2;
    Bg_SetupLayers(0, 0, resource, 0, 0, 1, &value);
    BgGfx_LoadTilemapSimple(0, 0, -4, 0, -24);
    return 0;
}
