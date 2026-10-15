#include "scene_inventory_control.h"
#define SceneInventory_TickModeMenu sub_08061DA0
s32 SceneInventory_TickModeMenu(struct SceneInventoryControlView *panel)
{
    SceneInputPanel_PositionModeCursor((void *)panel, panel->mode);
    if (panel->mode == 3)
        SceneInput_SetGlyphPair(&panel->prompt, 0, 4);
    else
        SceneInput_SetGlyphPair(&panel->prompt, 0, -1);
    SceneInputPanel_HighlightMode((void *)panel,
                                  ((struct SceneInventoryHighlightView *)panel)->colors);
    if (sub_08061C3C(panel))
        goto handled;
    if (sub_08061CFC(panel))
        goto handled;
    if (gUnk_03005260[1] & 2)
        return 1;
    if (gUnk_03005260[1] & 1) {
        switch (panel->mode) {
            case 4:
                Sound_PlaySong(0x107);
                sub_0806158C(panel);
                goto handled;
            case 1:
                Sound_PlaySong(0x107);
                sub_080615D8(panel);
                goto handled;
            case 3:
                Sound_PlaySong(0x107);
                sub_0806186C(panel);
                goto handled;
            case 2:
                Sound_PlaySong(0x107);
                sub_08061600(panel);
                goto handled;
        }
    }
    switch (gUnk_03005260[1] & 0xC0) {
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
