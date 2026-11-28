#include "global.h"
#include "system/game_state/game_state.h"

struct StageResults {
    u8 pad0[0x11C0];
    u32 summaryScript;
    u8 pad11C4[8];
    u32 textWindows[3];
};

void TextWindow_SetScript(u32, u32);
void TextWindow_Refresh(u32, u32);
void TextWindow_SetString(u32, u32, void *);
void TextWindow_StartPlayback(u32);

void StageResults_ShowSummaryText(struct StageResults *results)
{
    u8 *name = &gGameState->playerName[16];
    TextWindow_SetScript(results->textWindows[0], results->summaryScript);
    TextWindow_Refresh(results->textWindows[0], 0);
    TextWindow_SetString(results->textWindows[0], 1, name);
    TextWindow_StartPlayback(results->textWindows[0]);
}
