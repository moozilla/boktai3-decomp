#include "ui/title/options.h"
#include "gba/io_reg.h"
#include "engine/video/palette_pulse.h"

void Options_UpdateScreen(struct OptionsScreen *p)
{
    s32 value;
    struct ScriptUtilityArguments message;
    struct SceneInputPackedPosition rect;
    message.count = 1;
    message.values = &value;
    switch (p->state) {
        case 0:
            if (gPadState[1] & A_BUTTON) {
                if (p->selection == 0) {
                    Sound_PlaySong(0xDD);
                    p->fade = 0;
                    Menu_StartFadeOut(0);
                    p->state = 1;
                    p->exitChoice = 1;
                    break;
                } else if (p->selection == 1) {
                    Sound_PlaySong(0xDD);
                    p->fade = 0;
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
            if (gPadState[1] & B_BUTTON) {
                Sound_PlaySong(0xDE);
                p->fade = 3;
                Menu_StartFadeOut(3);
                p->state = 1;
                p->exitChoice = 0;
                break;
            }
            if (gPadState[1] & DPAD_UP) {
                Sound_PlaySong(0xDC);
                if (--p->selection < 0)
                    p->selection = 2;
            }
            if (gPadState[1] & DPAD_DOWN) {
                Sound_PlaySong(0xDC);
                if (++p->selection > 2)
                    p->selection = 0;
            }
            if (p->selection == 2) {
                if (gPadState[1] & DPAD_LEFT) {
                    Sound_PlaySong(0xDC);
                    if (--p->choice < 0)
                        p->choice = 8;
                    Menu_EraseRect(9, 12, 18, 2);
                    Menu_DrawStringRef(9, 12, gChoiceMenuParameters.eBlock, p->choice);
                }
                if (gPadState[1] & DPAD_RIGHT) {
                    Sound_PlaySong(0xDC);
                    if (++p->choice > 8)
                        p->choice = 0;
                    Menu_EraseRect(9, 12, 18, 2);
                    Menu_DrawStringRef(9, 12, gChoiceMenuParameters.eBlock, p->choice);
                }
                gChoiceMenuParameters.n2 = p->choice;
            }
            rect.x = 0;
            rect.y = p->selection * 32 + 32;
            rect.z = 0;
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
            if (p->timer > (1 << p->fade)) {
                p->rightArrow.sprite.flags |= 1;
                p->leftArrow.sprite.flags |= 1;
                p->cursor.sprite.flags |= 1;
                p->menu.sprite.flags |= 1;
                p->background.sprite.flags |= 1;
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
    u16 *originalPalette;
    s32 layer;
    struct OptionsTextRect rect;
    void *background;
    void *palette;
    void *resource;
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    background = ScriptProc_Start(0xC091, 0x3536);
    layer = 9;
    Bg_SetupLayers(2, 0, background, 0, 0, 1, &layer);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    palette = ScriptProc_Start(0x92B3, 0x204);
    CpuSet(((struct OptionsPalette *)palette)->colors, gBgPaletteBuffer, 0x100);
    resource = ScriptProc_Start(0xCB05, 0xE2AB);
    ResourceHeader_Relocate(&screen->background.graphics, resource);
    AnimSprite_InitLinked(&screen->background.sprite, &screen->background.graphics, 0x3A, 0x10, 0,
                          0, 0, 0);
    resource = ScriptProc_Start(0xCB05, 0x530D);
    ResourceHeader_Relocate(&screen->menu.graphics, resource);
    AnimSprite_InitLinked(&screen->menu.sprite, &screen->menu.graphics, 0x8A, 0x10, 0, 0, 0, 0);
    ResourceHeader_Relocate(&screen->cursor.graphics, resource);
    AnimSprite_InitLinked(&screen->cursor.sprite, &screen->cursor.graphics, 0x47, 0x10, 0, 0, 0, 0);
    ResourceHeader_Relocate(&screen->leftArrow.graphics, resource);
    AnimSprite_InitLinked(&screen->leftArrow.sprite, &screen->leftArrow.graphics, 0x45, 0x10, 0, 0,
                          0, 0);
    rect.x = 0x40;
    rect.y = 0x60;
    rect.width = 0;
    Menu_SetTextRect(&screen->leftArrow.sprite, &rect);
    ResourceHeader_Relocate(&screen->rightArrow.graphics, resource);
    AnimSprite_InitLinked(&screen->rightArrow.sprite, &screen->rightArrow.graphics, 0x46, 0x10, 0,
                          0, 0, 0);
    rect.x = 0xD8;
    rect.y = 0x60;
    rect.width = 0;
    Menu_SetTextRect(&screen->rightArrow.sprite, &rect);
    screen->cursor.sprite.flags |= 1;
    screen->leftArrow.sprite.flags |= 1;
    screen->rightArrow.sprite.flags |= 1;
    originalPalette = screen->originalPalette = screen->cursor.sprite.palette;
    CpuSet(originalPalette, screen->highlightPalette, 0x10);
    screen->cursor.sprite.palette = screen->highlightPalette;
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
        screen->fade = 3;
    else
        screen->fade = 0;
    Menu_StartFadeIn(screen->fade);
    screen->selection = gChoiceMenuParameters.n;
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
