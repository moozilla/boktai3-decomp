#include "global.h"
#include "render.h"
#include "scene_input.h"
#include "menu_parameters.h"
// confidence=low: background, labels, animations and menu selection initialization.
struct OptionsTextRect {
    u16 x, y, width, height;
};
void Bg_SetupLayers(s32, s32, void *, s32, s32, s32, void *);
void Menu_SetTextRect(void *, void *);
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawStringRef(s32, s32, const u8 *, s32);
void Menu_StartFadeIn(s32);
struct OptionsWidget {
    struct SceneInputWidget sprite;
    struct SceneInputResource graphics;
    u8 unknown80[8];
};
struct OptionsScreen {
    u8 task[0x1C];
    s32 selection, choice;
    struct OptionsWidget background, menu, cursor, leftArrow, rightArrow;
    u8 unknown2CC[0xC];
    u32 fade;
    u16 *originalPalette;
    u16 highlightPalette[16];
};
s32 Options_InitScreen(struct OptionsScreen *screen)
{
    struct SceneInputResource *backgroundGraphics;
    struct SceneInputResource *cursorGraphics;
    struct SceneInputResource *leftArrowGraphics;
    struct SceneInputResource *menuGraphics;
    struct SceneInputWidget *leftArrow;
    struct SceneInputWidget *rightArrow;
    u16 *highlightPalette;
    u16 *originalPalette;
    s32 layer;
    struct OptionsScreen *state;
    struct OptionsTextRect rect;
    u32 one;
    u32 zero;
    void *background;
    void *palette;
    void *resource;
    state = screen;
    Bg_ClearTilemap(0);
    zero = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    background = ScriptProc_Start(0xC091, 0x3536);
    layer = 9;
    Bg_SetupLayers(2, 0, background, 0, zero, one = 1, &layer);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    palette = ScriptProc_Start(0x92B3, 0x204);
    CpuSet(((u8 *)palette) + 0x14, gBgPaletteBuffer, 0x100);
    resource = ScriptProc_Start(0xCB05, 0xE2AB);
    backgroundGraphics = &screen->background.graphics;
    ResourceHeader_Relocate(backgroundGraphics, resource);
    Widget_InitLinked(&screen->background.sprite, backgroundGraphics, 0x3A, 0x10, zero, zero, zero,
                      0);
    resource = ScriptProc_Start(0xCB05, 0x530D);
    menuGraphics = &screen->menu.graphics;
    ResourceHeader_Relocate(menuGraphics, resource);
    Widget_InitLinked(&screen->menu.sprite, menuGraphics, 0x8A, 0x10, zero, zero, zero, 0);
    cursorGraphics = &screen->cursor.graphics;
    ResourceHeader_Relocate(cursorGraphics, resource);
    Widget_InitLinked(&screen->cursor.sprite, cursorGraphics, 0x47, 0x10, zero, zero, zero, 0);
    leftArrowGraphics = &screen->leftArrow.graphics;
    ResourceHeader_Relocate(leftArrowGraphics, resource);
    leftArrow = &screen->leftArrow.sprite;
    Widget_InitLinked(leftArrow, leftArrowGraphics, 0x45, 0x10, zero, zero, zero, 0);
    rect.x = 0x40;
    rect.y = 0x60;
    rect.width = 0;
    Menu_SetTextRect(leftArrow, &rect);
    ResourceHeader_Relocate(&screen->rightArrow.graphics, resource);
    rightArrow = &screen->rightArrow.sprite;
    Widget_InitLinked(rightArrow, &screen->rightArrow.graphics, 0x46, 0x10, zero, zero, zero, 0);
    rect.x = 0xD8;
    rect.y = 0x60;
    rect.width = 0;
    Menu_SetTextRect(rightArrow, &rect);
    screen->cursor.sprite.flags |= one;
    screen->leftArrow.sprite.flags |= one;
    screen->rightArrow.sprite.flags |= one;
    originalPalette = screen->originalPalette = screen->cursor.sprite.palette;
    highlightPalette = screen->highlightPalette;
    CpuSet(originalPalette, highlightPalette, 0x10);
    screen->cursor.sprite.palette = highlightPalette;
    screen->cursor.sprite.paletteBank = 0xE010;
    Menu_EraseRect(4, 4, 24, 6);
    Menu_EraseRect(4, 12, 4, 2);
    Menu_DrawStringRef(4, 4, gChoiceMenuParameters.mBlock, 0);
    Menu_DrawStringRef(4, 8, gChoiceMenuParameters.mBlock, 1);
    Menu_DrawStringRef(4, 12, gChoiceMenuParameters.mBlock, 2);
    screen->choice = gChoiceMenuParameters.n2;
    Menu_EraseRect(9, 12, 18, 2);
    Menu_DrawStringRef(9, 12, gChoiceMenuParameters.eBlock, screen->choice);
    if ((gChoiceMenuParameters.b == 0) || (gChoiceMenuParameters.n == 2))
        state->fade = 3;
    else
        state->fade = zero;
    Menu_StartFadeIn(state->fade);
    state->selection = gChoiceMenuParameters.n;
    return 0;
}
