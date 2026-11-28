#include "global.h"
#include "ui/results/stage_results.h"
#include "engine/engine.h"
#include "engine/video/render.h"

void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);

void StageResults_LoadBackgroundResources(struct StageResults *results)
{
    u32 size;
    results->layerMap = ScriptProc_Start(0xC091, 0xCE15);
    results->layerData = ScriptProc_Start(0xC091, 0xA597);
    size = 4;
    Bg_SetupLayers(2, 0, results->layerMap, 0, 0, 1, &size);
    results->palette = (u8 *)ScriptProc_Start(0x92B3, 0xA2F6) + 0x14;
    CpuSet(results->palette, gBgPaletteBuffer, 0x100);
}
