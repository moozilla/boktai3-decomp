#include "pause_menu.h"
#include "message_window.h"
#include "scrolling_dialogue.h"
#include "player_cue_sequence.h"

void PauseMenu_ClearTileRect(s32 x, s32 y, s32 width, s32 height)
{
    s32 row;
    for (row = 0; row < height; row++) {
        u16 *cell = Hud_GetTilemapCell(0, x, y + row);
        s32 col;
        for (col = 0; col < width; col++) {
            *cell++ = 0xf001;
        }
    }
}

void PauseMenu_SetSpritePalette(struct SceneInputWidget *widget, u32 bank)
{
    widget->paletteBank = bank;
    widget->palette = (u16 *)((u8 *)gPaletteBankData + (widget->paletteBank << 5));
}

void PauseMenu_SmoothPoint(s16 *position, s16 *target, s32 weight)
{
    s32 divisor;
    position[0] = Div(position[0] * weight + target[0], divisor = weight + 1);
    position[1] = Div(position[1] * weight + target[1], divisor);
}
