#include "minigames/result_game/result_tutorial.h"
extern u16 gUnknown_03005260[];
void ResultMenu_SendCommand(void *, int, int, void *);
void ResultGame_ScorePress(struct ResultMenu *menu);
void ResultGame_ShowPressMarker(struct ResultMenu *menu);

void ResultGame_HandlePress(struct ResultMenu *menu)
{
    if ((s8)menu->pressState == 0 && (gUnknown_03005260[1] & 1)) {
        ResultMenu_SendCommand(menu, 0x34, 0, 0);
        menu->pressState = 1;
        menu->pressed = 1;
        ResultGame_ScorePress(menu);
        menu->pressScore = menu->playerPosition >> 5;
        ResultGame_ShowPressMarker(menu);
    }
}
