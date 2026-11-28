#include "global.h"
#include "ui/results/stage_results.h"
#include "system/game_state/game_state.h"
#include "engine/input.h"

void Bg_SetLayerMap(u32, void *, u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
s32 TextWindow_SetScript(s32 id, const u8 *script);
s32 TextWindow_Refresh(s32 id, s32 index);
void TextWindow_StartPlayback(s32);
s32 GameState_GetResultTimer(void);
s32 GameState_GetResultIndex(void);
s32 GameState_GetBikeRecoveryCount(void);
s32 Div(s32, s32);
s32 SceneGauge_SelectThresholdFrame(struct StageResults *, s32);
u8 StageResults_RecordStagePerformance(s32);
void StageResults_ShowTimeDigits(struct StageResults *, s32, s32);
void CharacterModel_UpdatePairedAnimations(void *, s32);
void PaletteFade_CycleMenuColors(void *);
void StageResults_UpdateRecordBlink(struct StageResults *);
void Sound_PlaySong(u32);
void MessagePanel_UpdateExitDelay(struct StageResults *);

void sub_08112508(struct StageResults *results)
{
    if (results->entering) {
        s32 elapsed, timeScore, actionScore, total;
        results->isRecord = 0;
        Bg_SetLayerMap(3, results->layerMap, 4);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        TextWindow_SetScript(results->textWindows[1], results->labelScript);
        TextWindow_SetScript(results->textWindows[2], results->labelScript);
        TextWindow_Refresh(results->textWindows[1], 0);
        TextWindow_Refresh(results->textWindows[2], 1);
        TextWindow_StartPlayback(results->textWindows[1]);
        TextWindow_StartPlayback(results->textWindows[2]);
        elapsed = GameState_GetResultTimer();
        timeScore = Div(results->targetTimes[GameState_GetResultIndex()] - elapsed, 60) + 25;
        actionScore = Div(20, GameState_GetBikeRecoveryCount() + 1);
        timeScore = StageResults_Clamp(timeScore, 80);
        actionScore = StageResults_Clamp(actionScore, 20);
        total = timeScore + actionScore;
        total = StageResults_Clamp(total, 999);
        results->isRecord =
            StageResults_RecordStagePerformance(SceneGauge_SelectThresholdFrame(results, total));
        {
            s32 sum = StageResults_Clamp(gGameState->clockDisplayValue + total, 9999);
            s16 *score = &gGameState->clockDisplayValue;
            *score = sum;
        }
        StageResults_ShowTimeDigits(results, 8, elapsed);
        StageResults_ShowTimeDigits(results, 14, timeScore);
        StageResults_ShowTimeDigits(results, 17, actionScore);
        StageResults_ShowTimeDigits(results, 20, total);
        results->blinkTimer = 0;
        results->exitTimer = 0;
        results->entering = 0;
    }
    if (results->exitTimer > 9) {
        CharacterModel_UpdatePairedAnimations(results->model, 0);
        PaletteFade_CycleMenuColors(results->menuPalette);
        StageResults_UpdateRecordBlink(results);
        if (gPadState[1] & 0x30f) {
            Sound_PlaySong(0xdd);
            StageResults_SetUpdate(results, MessagePanel_UpdateExitDelay, 1);
        } else {
            results->exitTimer++;
        }
    } else {
        results->exitTimer++;
    }
}
