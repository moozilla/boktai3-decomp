#define SceneDialog_SelectResultPhase sub_081763D4
#define SceneDialog_SetPhase Hud_SetInteractionCallback
#define gSceneDialogActive            gUnk_03005408
#include "global.h"
#include "game_state.h"
extern s32 gUnk_03005408;
u8 Save_WriteAlternateSlotAndSelect(void);
void sub_08176424(void);
void sub_08176488(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_081763D4(void *p)
{
    gUnk_03005408 = 1;
    if (Save_WriteAlternateSlotAndSelect()) {
        gUnk_02000710->flag86E = 1;
        Hud_SetInteractionCallback(p, sub_08176424, 1);
    } else
        Hud_SetInteractionCallback(p, sub_08176488, 1);
}
