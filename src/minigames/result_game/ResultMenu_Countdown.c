#include "minigames/result_game/result_tutorial.h"
int ResultMenu_IsPhaseStart(void *);
void ResultMenu_SendCommand(void *, int, int, void *);
void ResultMenu_SetPhase(void *, void *);
void ResultGame_Update(void *);

void ResultMenu_Countdown(struct ResultMenu *menu)
{
    if (ResultMenu_IsPhaseStart(menu))
        ResultMenu_SendCommand(menu, 0x31, 0, 0);
    if (menu->phaseFrame > 239)
        ResultMenu_SetPhase(menu, ResultGame_Update);
    else {
        if (Mod(menu->phaseFrame, 61) == 60)
            Sound_PlaySong(0x21c);
        menu->phaseFrame++;
    }
}
