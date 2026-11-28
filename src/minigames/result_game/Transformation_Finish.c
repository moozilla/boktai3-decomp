#include "minigames/result_game/result_tutorial.h"
#include "global.h"
void ScreenTransition_SendCommand(int, int, int, int, int, int, int);
void Transformation_ReportResult(struct ResultMenu *menu);
void Transformation_Finish(struct ResultMenu *menu)
{
    if (menu->phaseFrame == 60)
        ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0xffff, 0);
    if (menu->phaseFrame > 91) {
        if (menu->completionScript)
            Transformation_ReportResult(menu);
        Task_Kill(menu);
    } else
        menu->phaseFrame++;
}
