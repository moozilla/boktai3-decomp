#include "minigames/result_game/result_tutorial.h"
void ResultGame_SetUpdateCallback(struct ResultMenu *menu, void (*callback)(struct ResultMenu *));
void ResultGame_TrackTarget(struct ResultMenu *menu);
int ResultMenu_IsPhaseStart(void *);
void ResultMenu_SetPhase(void *, void *);
void ResultGame_Finish(void *);

void ResultGame_Update(struct ResultMenu *menu)
{
    if (ResultMenu_IsPhaseStart(menu)) {
        ResultGame_SetUpdateCallback(menu, ResultGame_TrackTarget);
        menu->gaugePosition = 0;
        menu->unknown60 = 0;
        menu->unknown64 = 32;
        menu->nextFeedbackSlot = 0;
        menu->unknownD4 = 0;
        menu->preciseCount = 0;
        menu->closeCount = 0;
        menu->missedCount = 0;
        menu->unknown6C = 2;
        if ((u32)menu->level > 10)
            menu->level = 10;
        menu->startLevel = menu->level;
        menu->trackingScore = 0;
        menu->trackingSamples = 0;
        menu->pressScoreTotal = 0;
        menu->pressSamples = 0;
        menu->finishState = 0;
    }
    menu->unknownBD[1] = 0;
    Callback_InvokeActor(menu, menu->gameUpdate);
    if (menu->finishState == 2) {
        Sound_StopSong(0x3ed);
        Sound_StopSong(0x3ee);
        ResultMenu_SetPhase(menu, ResultGame_Finish);
    } else {
        if (menu->countdown == 0)
            menu->finishState = 1;
        else
            menu->countdown--;
        menu->phaseFrame++;
    }
}
