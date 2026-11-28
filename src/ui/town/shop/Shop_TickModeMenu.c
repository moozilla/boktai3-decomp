#include "ui/town/shop_control.h"
s32 Shop_TickModeMenu(struct ShopControlView *panel)
{
    SceneInputPanel_PositionModeCursor((void *)panel, panel->mode);
    if (panel->mode == 3)
        SceneInput_SetGlyphPair(&panel->prompt, 0, 4);
    else
        SceneInput_SetGlyphPair(&panel->prompt, 0, -1);
    SceneInputPanel_HighlightMode((void *)panel, ((struct ShopHighlightView *)panel)->colors);
    if (Shop_HandleClockShortcut(panel) || Shop_HandlePageKeys(panel))
        return 0;
    if (gPadState[1] & 2)
        return 1;
    if (gPadState[1] & 1) {
        switch (panel->mode) {
            case 4:
                Sound_PlaySong(0x107);
                Shop_BeginMoveSelection(panel);
                return 0;
            case 1:
                Sound_PlaySong(0x107);
                Shop_BeginItemSelection(panel);
                return 0;
            case 3:
                Sound_PlaySong(0x107);
                Shop_BeginExitConfirmation(panel);
                return 0;
            case 2:
                Sound_PlaySong(0x107);
                Shop_BeginConsumableSelection(panel);
                return 0;
        }
    }
    switch (gPadState[1] & 0xC0) {
        case 0x40:
            panel->modeIndex--;
            if (panel->modeIndex > 3)
                panel->modeIndex = 3;
            panel->mode = gUnk_0824DF38[panel->modeIndex];
            Sound_PlaySong(0xDC);
            break;
        case 0x80:
            panel->modeIndex++;
            if (panel->modeIndex > 3)
                panel->modeIndex = 0;
            panel->mode = gUnk_0824DF38[panel->modeIndex];
            Sound_PlaySong(0xDC);
            break;
    }
    return 0;
}
