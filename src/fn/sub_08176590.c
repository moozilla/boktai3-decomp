#define SceneDialog_ConfigureVariantSlot sub_08176590
#define SceneDialog_ClearSlotFlags PauseMenu_ClearSpriteFlags
#define gSceneDialogVariantCallbacks     gUnk_08611C34
#include "global.h"
struct SceneDialogVariant {
    u8 pad0[0x1414];
    u8 variant;
};
extern void *gUnk_08611C34[];
s32 sub_0824923C(void *, void *);
void sub_081658C8(void *);
void PauseMenu_SetSpritePosition(void *, s32, s32, s32);
void PauseMenu_ClearSpriteFlags(void *, s32, u32);
void sub_08176590(struct SceneDialogVariant *p)
{
    sub_0824923C(p, gUnk_08611C34[p->variant]);
    sub_081658C8(p);
    PauseMenu_SetSpritePosition(p, 3, p->variant * 8, 0);
    PauseMenu_ClearSpriteFlags(p, 3, 1);
}
