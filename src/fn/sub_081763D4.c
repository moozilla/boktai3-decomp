#define SceneDialog_SetPhase Hud_SetInteractionCallback
#include "global.h"
#include "game_state.h"
#include "runtime.h"
u8 Save_WriteAlternateSlotAndSelect(void);
void SceneDialog_FinishSuccessfulResult(void);
void SceneDialog_FinishFailedResult(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void SceneDialog_SelectResultPhase(void *p)
{
    gSceneDialogActive = 1;
    if (Save_WriteAlternateSlotAndSelect()) {
        gGameState->flag86E = 1;
        Hud_SetInteractionCallback(p, SceneDialog_FinishSuccessfulResult, 1);
    } else
        Hud_SetInteractionCallback(p, SceneDialog_FinishFailedResult, 1);
}
