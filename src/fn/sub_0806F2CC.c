#include "engine.h"
void GameState_ClearSlots7A8FromScript(void);
void GameState_ClearChallengeCompletionFlags(void);
void GameState_ResetChallengeRecords(void);
static inline u16 *SceneState_HalfwordAt(u8 *base, s32 offset)
{
    return (u16 *)(base + offset);
}

void SceneState_ResetRuntimeFields(void)
{
    *SceneState_HalfwordAt((u8 *)gGameState, 0x7A0) = 0;
    *SceneState_HalfwordAt((u8 *)gGameState, 0x7A2) = 0;
    *SceneState_HalfwordAt((u8 *)gGameState, 0x7A4) = 0;
    *((s16 *)(((u8 *)gGameState) + 0x7A6)) = -1;
    *SceneState_HalfwordAt((u8 *)gGameState, 0x81A) = 0;
    *SceneState_HalfwordAt((u8 *)gGameState, 0x7B2) = 0;
    *SceneState_HalfwordAt((u8 *)gGameState, 0x7B4) = 0;
    *SceneState_HalfwordAt((u8 *)gGameState, 0x7B6) = 0;
    *SceneState_HalfwordAt((u8 *)gGameState, 0x7FA) = 0;
    *SceneState_HalfwordAt((u8 *)gGameState, 0x7FC) = 0;
    *SceneState_HalfwordAt((u8 *)gGameState, 0x7FE) = 0;
    *((u16 *)(((u8 *)gGameState) + 0x818)) = 2000;
    *((u32 *)(((u8 *)gGameState) + 0x7B8)) = 1;
    *((u32 *)(((u8 *)gGameState) + 0x7BC)) = 1;
    *((u32 *)(((u8 *)gGameState) + 0x7C0)) = 1;
    *((u32 *)(((u8 *)gGameState) + 0x7C4)) = 0;
    *((u16 *)(((u8 *)gGameState) + 0x7F8)) = 1;
    *((u32 *)(((u8 *)gGameState) + 0x81C)) = 0;
    GameState_ClearSlots7A8FromScript();
    GameState_ClearChallengeCompletionFlags();
    GameState_ResetChallengeRecords();
    *((u32 *)(((u8 *)gGameState) + 0x820)) = 0;
}
