#include "global.h"
#include "minigames/result_game/result_tutorial.h"
int ResultMenu_IsPhaseStart(void *);
void ResultMenu_SendCommand(void *, int, int, void *);
void ResultMenu_SetPhase(void *, void *);
void Transformation_Charge(void *);
void ResultGame_Finish(struct ResultMenu *menu)
{
    if (ResultMenu_IsPhaseStart(menu)) {
        ResultMenu_SendCommand(menu, 0x37, 0, 0);
        Sound_PlayTrackedSong(0xa9);
    }
    if (menu->phaseFrame > 29)
        ResultMenu_SetPhase(menu, Transformation_Charge);
    else
        menu->phaseFrame++;
}
