#ifndef GUARD_GAME_OVER_H
#define GUARD_GAME_OVER_H
#include "scene_input.h"
struct GameOverSprite {
    u32 flags;
    u8 unknown4[3], priority, scaleX, scaleY, unknownA[6];
    u16 frame;
    u8 unknown12[10];
    struct ContactPosition position;
    u8 unknown24[8], animation[0x1C];
    s16 baseX, baseY;
    u8 unknown4C[4];
};
struct GameOver {
    struct Task task;
    struct GameOverSprite sprites[8];
    struct SceneInputResource overlayResource;
    struct SceneInputWidget overlay;
    u16 overlayPalette[16];
    void *text;
    s16 extentX, extentY;
    u8 closeState, animationState, graphicsLinked, choice;
    u8 unknown344[2];
    u16 timer, costs[2];
    u8 costIndex, pulseFrame, unknown34E[2];
    u32 procedure;
};
struct GameOverScriptArgs {
    u32 count : 16, unused : 16;
    s32 *values;
};
void GameOver_InitExpansion(struct GameOver *panel);
void GameOver_UpdateExpansion(struct GameOver *panel);
void GameOver_UpdateLetterRebound(struct GameOver *panel);
void GameOver_UpdateLetterSettle(struct GameOver *panel);
void GameOver_UpdateLetterExpand(struct GameOver *panel);
void GameOver_UpdateLetterHold(struct GameOver *panel);
void GameOver_UpdateOpening(struct GameOver *panel);
void GameOver_UpdateExitDelay(struct GameOver *panel);
void GameOver_UpdateLetterTransforms(struct GameOver *panel);
void GameOver_StopPresentation(void);
void GameOver_UpdateLetterAnimation(struct GameOver *panel);
void GameOver_ShowGraphics(struct GameOver *panel);
void GameOver_InitSprites(struct GameOver *panel);
void GameOver_InitOverlay(struct GameOver *panel);
void GameOver_StartFromScript(void);
void GameOver_TryBeginClose(struct GameOver *panel);
void GameOver_FinishDelay(struct GameOver *panel);
void GameOver_UpdateCostPrompt(struct GameOver *panel);
void GameOver_UpdateCostSelection(struct GameOver *panel);
void GameOver_UpdateExit(struct GameOver *panel);
s32 GameOver_OnUpdate(struct GameOver *panel);
u32 GameOver_OnDestroy(struct GameOver *panel);
s32 GameOver_Initialize(struct GameOver *panel);
void *GameOver_Create(void);
#endif
