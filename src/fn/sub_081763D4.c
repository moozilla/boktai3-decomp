#define SceneDialog_SetPhase Hud_SetInteractionCallback
#define gSceneDialogActive   gUnk_03005408
#include "global.h"
#include "game_state.h"
extern s32 gUnk_03005408;
u8 Save_WriteAlternateSlotAndSelect(void);
void SceneDialog_FinishSuccessfulResult(void);
void SceneDialog_FinishFailedResult(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void SceneDialog_SelectResultPhase(void *p)
{
    gUnk_03005408 = 1;
    if (Save_WriteAlternateSlotAndSelect()) {
        gGameState->flag86E = 1;
        Hud_SetInteractionCallback(p, SceneDialog_FinishSuccessfulResult, 1);
    } else
        Hud_SetInteractionCallback(p, SceneDialog_FinishFailedResult, 1);
}
