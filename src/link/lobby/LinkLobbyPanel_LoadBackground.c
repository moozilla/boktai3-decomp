#include "global.h"
#include "gba/syscall.h"
#include "gba/macro.h"
#include "engine/engine.h"
#include "engine/video/render.h"
#include "link/lobby/record_menu.h"

void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);

void LinkLobbyPanel_LoadBackground(struct RecordMenu *menu)
{
    u32 layer;

    menu->backgroundMap = ScriptProc_Start(0xC091, 0xCE15);
    menu->otherMap = ScriptProc_Start(0xC091, 0xA597);
    layer = 2;
    Bg_SetupLayers(2, 0, menu->backgroundMap, 0, 0, 1, &layer);
    menu->paletteResource = (u8 *)ScriptProc_Start(0x92B3, 0xA2F6) + 0x14;
    CpuCopy16(menu->paletteResource, gBgPaletteBuffer, 0x200);
}
