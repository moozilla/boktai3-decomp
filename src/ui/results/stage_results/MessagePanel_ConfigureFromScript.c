#include "global.h"
#include "ui/results/stage_results.h"

s32 Script_SeekToKeyword(u32);
u8 *Script_GetPc(void);
s32 Script_GetValueSafe(void);
s32 Script_GetValue(void);
void Script_SetPc(s32);
s32 Script_GetKeywordValue(u32, s32);
u32 Script_GetTextSourcePc(void);
void Bg_ClearTilemap(s32);
void StageResults_LoadBackgroundResources(struct StageResults *);
void StageResults_InitAnimationIds(struct StageResults *);
void MessagePanel_InitSprites(struct StageResults *);
void StageResults_InitPortraitDisplay(struct StageResults *);
void StageResults_InitCursorPositions(struct StageResults *);
void ModifierMenu_InitSelectionSnapshot(void *, void *);
void StageResults_CreateTextWindows(struct StageResults *);
void StageResults_ShowSummaryText(struct StageResults *);
void sub_08112508(struct StageResults *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
s32 MessagePanel_ConfigureFromScript(struct StageResults *results)
{
    s32 i;
    s32 value;
    if (Script_SeekToKeyword(0x6E)) {
        results->summaryScript = Script_GetPc();
        if (!results->summaryScript)
            return -1;
    }
    if (Script_SeekToKeyword(0x72)) {
        results->labelScript = (const u8 *)Script_GetTextSourcePc();
        if (!results->labelScript)
            return -1;
    }
    if (Script_SeekToKeyword(0x62)) {
        s32 scriptPc = Script_GetValueSafe();
        if (!scriptPc) {
            for (i = 0; i < 12; i++)
                results->targetTimes[i] = 6000;
        } else {
            Script_SetPc(scriptPc);
            for (i = 0; i <= 10; i++) {
                value = Script_GetValue();
                if (value < 0)
                    break;
                results->targetTimes[i] = value;
            }
        }
    }
    results->exitScript = Script_GetKeywordValue(0x65, 0);
    Bg_ClearTilemap(3);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(0);
    StageResults_LoadBackgroundResources(results);
    StageResults_InitAnimationIds(results);
    MessagePanel_InitSprites(results);
    StageResults_InitPortraitDisplay(results);
    StageResults_InitCursorPositions(results);
    ModifierMenu_InitSelectionSnapshot(results->menuPalette, results->palette);
    StageResults_CreateTextWindows(results);
    StageResults_ShowSummaryText(results);
    StageResults_SetUpdate(results, sub_08112508, 0);
    if (Script_GetKeywordValue(0x66, 0))
        ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, 0);
    return 1;
}
