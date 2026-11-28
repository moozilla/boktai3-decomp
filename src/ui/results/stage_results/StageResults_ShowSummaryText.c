#include "global.h"
#include "ui/results/stage_results.h"
#include "system/game_state/game_state.h"

void TextWindow_SetScript(s32, u8 *);
void TextWindow_Refresh(s32, s32);
void TextWindow_SetString(s32, s32, u8 *);
void TextWindow_StartPlayback(s32);

void StageResults_ShowSummaryText(struct StageResults *results)
{
    u8 *name = &gGameState->playerName[16];
    TextWindow_SetScript(results->textWindows[0], results->summaryScript);
    TextWindow_Refresh(results->textWindows[0], 0);
    TextWindow_SetString(results->textWindows[0], 1, name);
    TextWindow_StartPlayback(results->textWindows[0]);
}
