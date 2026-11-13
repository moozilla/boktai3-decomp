#include "options.h"

void Options_UpdateScreen(struct OptionsScreen *p)
{
    s32 value;
    s32 state;
    struct ScriptUtilityArguments message;
    struct SceneInputPackedPosition rect;
    message.count = 1;
    message.values = &value;
    switch (state = p->state) {
        case 0:
            if (gPadState[1] & 1) {
                if (p->selection == 0) {
                    Sound_PlaySong(0xDD);
                    p->fade = state;
                    Menu_StartFadeOut(0);
                    p->state = 1;
                    p->exitChoice = 1;
                    break;
                } else if (p->selection == 1) {
                    Sound_PlaySong(0xDD);
                    p->fade = state;
                    Menu_StartFadeOut(0);
                    p->state = 1;
                    p->exitChoice = 2;
                    break;
                } else if (p->selection == 2) {
                    Sound_PlaySong(0xDD);
                    p->fade = 2;
                    Menu_StartFadeOut(2);
                    p->state = 1;
                    p->exitChoice = 3;
                    break;
                }
            }
            if (gPadState[1] & 2) {
                Sound_PlaySong(0xDE);
                p->fade = 3;
                Menu_StartFadeOut(3);
                p->state = 1;
                p->exitChoice = 0;
                break;
            }
            if (gPadState[1] & 0x40) {
                Sound_PlaySong(0xDC);
                if (--p->selection < 0)
                    p->selection = 2;
            }
            if (gPadState[1] & 0x80) {
                Sound_PlaySong(0xDC);
                if (++p->selection > 2)
                    p->selection = 0;
            }
            if (p->selection == 2) {
                if (gPadState[1] & 0x20) {
                    Sound_PlaySong(0xDC);
                    if (--p->choice < 0)
                        p->choice = 8;
                    Menu_EraseRect(9, 12, 18, 2);
                    Menu_DrawStringRef(9, 12, gChoiceMenuParameters.eBlock, p->choice);
                }
                if (gPadState[1] & 0x10) {
                    Sound_PlaySong(0xDC);
                    if (++p->choice > 8)
                        p->choice = 0;
                    Menu_EraseRect(9, 12, 18, 2);
                    Menu_DrawStringRef(9, 12, gChoiceMenuParameters.eBlock, p->choice);
                }
                gChoiceMenuParameters.n2 = p->choice;
            }
            {
                rect.x = 0;
                rect.y = p->selection * 32 + 32;
                rect.z = 0;
            }
            Menu_SetTextRect(&p->cursor.sprite, &rect);
            p->cursor.sprite.flags &= ~1;
            Palette_UpdateHighlightPulse(p->highlightPalette + 12, &p->highlightPhase);
            if (p->selection == 2) {
                p->leftArrow.sprite.flags &= ~1;
                p->rightArrow.sprite.flags &= ~1;
            } else {
                p->leftArrow.sprite.flags |= 1;
                p->rightArrow.sprite.flags |= 1;
            }
            break;
        case 1:
            if (p->timer > (state << p->fade)) {
                p->rightArrow.sprite.flags |= state;
                p->leftArrow.sprite.flags |= state;
                p->cursor.sprite.flags |= state;
                p->menu.sprite.flags |= state;
                p->background.sprite.flags |= state;
                Script_Call(gChoiceMenuParameters.p, 0);
                if (p->exitChoice == 0)
                    Script_Call(gChoiceMenuParameters.c, 0);
                else if (p->exitChoice == 1)
                    Script_Call(gChoiceMenuParameters.s, 0);
                else if (p->exitChoice == 2)
                    Script_Call(gChoiceMenuParameters.r, 0);
                else {
                    value = p->choice;
                    Script_Call(gChoiceMenuParameters.x, &message);
                }
                Task_Kill(p);
            }
            p->timer++;
            break;
        case 2:
            break;
    }
}

void Options_Destroy(struct OptionsScreen *p)
{
    Anim_Remove(&p->rightArrow.sprite);
    Anim_Remove(&p->leftArrow.sprite);
    Anim_Remove(&p->cursor.sprite);
    Anim_Remove(&p->menu.sprite);
    Anim_Remove(&p->background.sprite);
}

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
    CpuSet(((struct OptionsPalette *)palette)->colors, gBgPaletteBuffer, 0x100);
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

void Options_SetParametersFromScript(void)
{
    struct ChoiceMenuParameters *p = &gChoiceMenuParameters;
    p->b = 0;
    p->n2 = 0;
    if (Script_SeekToKeyword('m'))
        p->mBlock = Script_GetPc();
    if (Script_SeekToKeyword('e'))
        p->eBlock = Script_GetPc();
    if (Script_SeekToKeyword('s'))
        p->s = Script_GetValue();
    if (Script_SeekToKeyword('r'))
        p->r = Script_GetValue();
    if (Script_SeekToKeyword('x'))
        p->x = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        p->c = Script_GetValue();
    if (Script_SeekToKeyword('n')) {
        p->n = Script_GetValue();
        p->n2 = Script_GetValue();
    }
    if (Script_SeekToKeyword('b'))
        p->b = Script_GetValue();
    if (Script_SeekToKeyword('p'))
        p->p = Script_GetValue();
}

u32 Options_GetSecondaryChoice(void)
{
    return gChoiceMenuParameters.n2;
}

struct OptionsScreen *Options_Create(void)
{
    struct OptionsScreen *r = Task_Create(0xb, sizeof(*r));
    if (r != 0) {
        Task_SetCallbacks(r, (void (*)(void *))Options_UpdateScreen,
                          (void (*)(void *))Options_Destroy);
        if (Options_InitScreen(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
