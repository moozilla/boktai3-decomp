#define SceneDialog_ClearSlotFlags PauseMenu_ClearSpriteFlags
#define gSceneDialogVariantCallbacks     gUnk_08611C34
#include "global.h"
struct SceneDialogVariant {
    u8 pad0[0x1414];
    u8 variant;
};
extern void *gUnk_08611C34[];
s32 Callback_InvokeActor(void *, void *);
void SceneHud_DrawSelectedClock(void *);
void PauseMenu_SetSpritePosition(void *, s32, s32, s32);
void PauseMenu_ClearSpriteFlags(void *, s32, u32);
void SceneDialog_ConfigureVariantSlot(struct SceneDialogVariant *p)
{
    Callback_InvokeActor(p, gUnk_08611C34[p->variant]);
    SceneHud_DrawSelectedClock(p);
    PauseMenu_SetSpritePosition(p, 3, p->variant * 8, 0);
    PauseMenu_ClearSpriteFlags(p, 3, 1);
}
