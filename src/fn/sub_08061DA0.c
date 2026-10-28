#include "scene_inventory_control.h"
s32 SceneInventory_TickModeMenu(struct SceneInventoryControlView *panel)
{
    SceneInputPanel_PositionModeCursor((void *)panel, panel->mode);
    if (panel->mode == 3)
        SceneInput_SetGlyphPair(&panel->prompt, 0, 4);
    else
        SceneInput_SetGlyphPair(&panel->prompt, 0, -1);
    SceneInputPanel_HighlightMode((void *)panel,
                                  ((struct SceneInventoryHighlightView *)panel)->colors);
    if (SceneInventory_HandleClockShortcut(panel))
        goto handled;
    if (SceneInventory_HandlePageKeys(panel))
        goto handled;
    if (gPadState[1] & 2)
        return 1;
    if (gPadState[1] & 1) {
        switch (panel->mode) {
            case 4:
                Sound_PlaySong(0x107);
                SceneInventory_BeginMoveSelection(panel);
                goto handled;
            case 1:
                Sound_PlaySong(0x107);
                SceneInventory_BeginItemSelection(panel);
                goto handled;
            case 3:
                Sound_PlaySong(0x107);
                SceneInventory_BeginExitConfirmation(panel);
                goto handled;
            case 2:
                Sound_PlaySong(0x107);
                SceneInventory_BeginConsumableSelection(panel);
                goto handled;
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
handled:
    return 0;
}
