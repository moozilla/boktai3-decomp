#include "global.h"
#include "render.h"
void SelectionPanel_ClearTileColumns(u8 *);
void SceneHud_ApplySecondarySelection(u8 *);
void PauseMenu_ApplySelectedEquipment(u8 *);
void PauseMenu_ApplySelectedVariant(u8 *);
void SceneHud_RefreshWidgetVisibility(u8 *);
void Sound_RestartPlayer29And30Songs(void);
void Hud_SetUpdateCallback(u8 *, void *);
void Hud_UpdateEquipmentPrompt(void);
static inline void andm(u32 *a, u32 m)
{
    *a &= m;
}
void PauseMenu_FinishEquipmentSelection(u8 *p)
{
    SelectionPanel_ClearTileColumns(p);
    SceneHud_ApplySecondarySelection(p);
    PauseMenu_ApplySelectedEquipment(p);
    PauseMenu_ApplySelectedVariant(p);
    p[0xC73] = 0;
    SceneHud_RefreshWidgetVisibility(p);
    Sound_RestartPlayer29And30Songs();
    andm(&gEnginePauseFlags, ~4);
    Hud_SetUpdateCallback(p, Hud_UpdateEquipmentPrompt);
}
