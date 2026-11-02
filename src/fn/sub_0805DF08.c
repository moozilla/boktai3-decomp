#include "scene_hud_display.h"

void SceneHud_DrawNumericStatus(u32 mode)
{
    u16 *output = SceneHud_GetMapCell(0, 20, 1);
    if (mode == 0) {
        *output++ = 0xF0C0;
        *output++ = 0xF0BB;
        *output++ = 0xF0B2;
        output[0] = 0xF061;
        output[1] = 0xF001;
        SceneHud_DrawDecimal(gGameState->unk7B0, 10000, 24, 1, mode);
    } else if (mode == 1) {
        *output++ = 0xF0BA;
        *output++ = 0xF0D0;
        *output++ = 0xF0BB;
        output[0] = 0xF061;
        output[1] = 0xF020;
        SceneHud_DrawDecimal(((struct SceneHudNumericView *)gGameState)->value50C, 10000, 24, 1,
                             mode);
    } else {
        *output++ = 0xF0B5;
        *output++ = 0xF0B6;
        *output++ = 0xF0BB;
        output[0] = 0xF0BF;
        output[1] = 0xF061;
        SceneHud_DrawDecimal(gGameState->unk77C, 10000, 24, 1, 0);
    }
}
