#include "timed_presentation.h"
#include "input.h"
#include "tether_presentation.h"

// Provisional name: TimedPresentation_Update

void Menu_StartFadeOut(s32);

s32 Heap_IsEndGuardValid(void);

// Wait for the sequence duration or an eligible skip, then finish its exit fade.

void TimedPresentation_Update(struct TimedPresentation *presentation)
{
    switch (presentation->state) {
        case 0:
            if (presentation->timer > presentation->duration - (1 << presentation->fadeShift)) {
                presentation->state = 1;
                Menu_StartFadeOut(presentation->fadeShift);
            } else if (presentation->timer > (1 << presentation->minShift) &&
                       Heap_IsEndGuardValid() && (gPadState[1] & 1)) {
                presentation->state = 1;
                Menu_StartFadeOut(presentation->fadeShift);
            }
            break;
        case 1:
            if (presentation->exitTimer > (1 << presentation->fadeShift)) {
                Script_Call(presentation->exitScript, 0);
                Task_Kill(presentation);
            }
            presentation->exitTimer++;
            break;
        case 2:
            break;
    }
    presentation->timer++;
}

void TimedPresentation_Destroy(void)
{
    u16 *r = &gDispCntShadow;
    u32 m = ~3;
    *r = m & *r;
}

void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);

void Menu_StartFadeIn(u32);

s32 TimedPresentation_Initialize(struct TimedPresentation *presentation)
{
    u32 zero, value;
    void *map;
    struct ScreenPaletteResource *palette;
    presentation->minShift = 4;
    presentation->fadeShift = 4;
    presentation->duration = 300;
    if (Script_SeekToKeyword(0x66)) {
        presentation->minShift = Script_GetValue();
        presentation->fadeShift = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x64))
        presentation->duration = Script_GetValue();
    if (Script_SeekToKeyword(0x74))
        presentation->tileLayer = Script_GetValue();
    if (Script_SeekToKeyword(0x65))
        presentation->exitScript = Script_GetValue();
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero = 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    map = ScriptProc_Start(0xC091, 0x52AE);
    value = presentation->tileLayer;
    Bg_SetupLayers(2, 0, map, 0, zero, 1, &value);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    palette = ScriptProc_Start(0x92B3, 0xD985);
    CpuSet(palette->colors, gBgPaletteBuffer, 0x100);
    Menu_StartFadeIn(presentation->minShift);
    return 0;
}
