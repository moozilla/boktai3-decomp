#include "global.h"
#include "player.h"
#include "game_state.h"
struct StatDisplay {
    u8 unknown0[0xA40];
    struct Player *player;
    u8 unknownA44[0x28];
    u8 enabled;
};
s32 Div(s32, s32);
void StatDisplay_DrawRatio(struct StatDisplay *, s32, s32, s32, s32, s32, s32, s32);
void StatDisplay_DrawPlayerValue(struct StatDisplay *p, s32 style, s32 x, s32 y)
{
    if (p->enabled) {
        if ((u16)((u16)gGameState->form - 1) <= 1) {
            s32 current = Div(p->player->statusValue, 4);
            s32 maximum = Div(4000, 4);
            StatDisplay_DrawRatio(p, style, 1000, 4, current, maximum, x, y);
        } else {
            s32 divisor = 1000;
            StatDisplay_DrawRatio(p, style, divisor, 4, p->player->statHundredths, divisor, x, y);
        }
    }
}
