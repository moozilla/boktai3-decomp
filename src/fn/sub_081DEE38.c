#include "date_entry_menu.h"
#include "render.h"
#include "menu_parameters.h"

void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
s32 ResourceHeader_Relocate(void *, void *);
s32 Widget_InitLinked(u8 *, void *, u16, u32, u8, u8, u8, const struct ContactPosition *);
void Password_LoadDate(void *);
void AnimPanel_UpdateDigits(void *);
void Menu_StartFadeIn(u32);
static inline void RelocateDateCursor(struct DateMenu *menu, void *source, u8 **graphics)
{
    *graphics = menu->widgets[3].resource;
    ResourceHeader_Relocate(*graphics, source);
}

s32 Time_InitializeDateMenu(struct DateMenu *menu)
{
    u32 zero;
    u32 one;
    u32 resourceBank;
    void *resource;
    u8 *firstGraphics;
    u8 *secondGraphics;
    u8 *cursorGraphics;
    u32 layer;
    void *bg;
    u16 **firstPalette;
    u16 *colors;
    u16 firstPaletteBank;
    u16 *paletteBank;
    menu->state = zero = 0;
    menu->selection = zero;
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    bg = ScriptProc_Start(0xC091, 0x3536);
    layer = 2;
    Bg_SetupLayers(2, 0, bg, 0, zero, one = 1, &layer);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    CpuSet((u8 *)ScriptProc_Start(0x92B3, 0x204) + 0x14, gBgPaletteBuffer, 0x100);

    resourceBank = 0xCB05;
    resource = ScriptProc_Start(resourceBank, 0xE2AB);
    firstGraphics = menu->widgets[0].resource;
    ResourceHeader_Relocate(firstGraphics, resource);
    menu->digitResource = *(struct SceneInputResource *)firstGraphics;
    Widget_InitLinked((u8 *)&menu->widgets[0], firstGraphics, 0x34, 0x10, zero, zero, zero, NULL);
    secondGraphics = menu->widgets[1].resource;
    ResourceHeader_Relocate(secondGraphics, resource);
    Widget_InitLinked((u8 *)&menu->widgets[1], secondGraphics, 0x26, 0x10, zero, zero, zero, NULL);

    ResourceHeader_Relocate(menu->widgets[2].resource, resource);
    Widget_InitLinked((u8 *)&menu->widgets[2], menu->widgets[2].resource, 0x27, 0x10, zero, zero,
                      zero, NULL);
    menu->widgets[1].flags |= one;
    menu->widgets[2].flags |= one;
    {
        u16 **savedPalette = &menu->savedPalette;
        u16 *sourcePalette;
        firstPalette = &menu->widgets[1].palette;
        sourcePalette = *firstPalette;
        *savedPalette = sourcePalette;
        colors = menu->colors;
        CpuSet(sourcePalette, colors, 0x10);
        *firstPalette = colors;
    }
    menu->widgets[2].palette = colors;
    paletteBank = &menu->widgets[1].paletteBank;
    firstPaletteBank = 0xE000;
    *paletteBank = firstPaletteBank;
    paletteBank = &menu->widgets[2].paletteBank;
    *paletteBank = firstPaletteBank + 1;
    resource = ScriptProc_Start(resourceBank, 0x530D);
    RelocateDateCursor(menu, resource, &cursorGraphics);
    Widget_InitLinked((u8 *)&menu->widgets[3], cursorGraphics, 0x80, 0x10, zero, zero, zero, NULL);
    sub_081DE630(menu);
    Password_LoadDate(menu);
    AnimPanel_UpdateDigits(menu);
    if (gDateMenuParameters.flag == 1) {
        menu->fadeShift = zero;
    } else {
        menu->fadeShift = 3;
    }
    Menu_StartFadeIn(menu->fadeShift);
    return 0;
}
