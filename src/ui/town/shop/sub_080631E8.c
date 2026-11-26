#include "ui/town/shop_lifecycle.h"
void Shop_UpdateDisplay(struct SceneInventoryTail *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void SceneInputPanel_RequestAction(struct SceneInventoryTail *,
                                   s32 (*)(struct SceneInventoryTail *));
s32 Shop_FinishAndCallScript(struct SceneInventoryTail *);
s32 Shop_RunController(struct SceneInventoryTail *p)
{
    if (Callback_InvokeActor(p, ((struct SceneInventoryLifecycle *)p)->callback) == 0) {
        Shop_UpdateDisplay(p);
    } else {
        Sound_PlaySong(0x119);
        ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xFFFF, 0);
        SceneInputPanel_RequestAction(p, Shop_FinishAndCallScript);
    }
    return 0;
}
