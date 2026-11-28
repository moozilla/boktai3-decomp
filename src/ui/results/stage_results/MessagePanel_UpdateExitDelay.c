#include "global.h"
#include "ui/results/stage_results.h"

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void CharacterModel_UpdatePairedAnimations(void *, s32);
void PaletteFade_CycleMenuColors(void *);
void StageResults_UpdateRecordBlink(struct StageResults *);
void StageResults_FinishDisplay(struct StageResults *);
void ScriptDisplay_OnTransitionEnd(struct StageResults *);

void MessagePanel_UpdateExitDelay(struct StageResults *results)
{
    if (results->entering) {
        ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xFFFF, 0);
        results->exitTimer = 0;
        results->entering = 0;
    }
    CharacterModel_UpdatePairedAnimations(results->model, 0);
    PaletteFade_CycleMenuColors(results->menuPalette);
    StageResults_UpdateRecordBlink(results);
    if (results->exitTimer <= 31) {
        results->exitTimer++;
    } else {
        StageResults_FinishDisplay(results);
        StageResults_SetUpdate(results, ScriptDisplay_OnTransitionEnd, 2);
    }
}
