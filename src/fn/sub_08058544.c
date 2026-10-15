#include "scene_panel.h"
s32 ScenePanel_GetPaletteOffset(struct ScenePanel *panel)
{
    s32 result;
    switch (panel->paletteMode) {
        case 0:
            result = 8;
            break;
        case 1:
        case 2:
            result = 6;
            break;
        case 3:
            result = 4;
            break;
        case 4:
            result = 2;
            break;
        case 7:
        case 8:
            result = 10;
            break;
        case 9:
        case 10:
            result = 12;
            break;
        default:
            result = 0;
            break;
    }
    return result;
}
