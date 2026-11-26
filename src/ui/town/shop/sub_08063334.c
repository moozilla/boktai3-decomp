#include "ui/town/shop_lifecycle.h"
void Shop_BuildUnlockedEquipmentCatalog(struct SceneInventoryLifecycleView *);
void Shop_BuildConsumableCatalog(struct SceneInventoryLifecycleView *);
void Shop_BuildUnlockedItemCatalog(struct SceneInventoryLifecycleView *);
void Shop_ReadScriptCatalog(struct SceneInventoryLifecycleView *);
void Shop_InitCatalogBackground(struct SceneInventoryLifecycleView *, u32);
void Shop_InitPanel(struct SceneInventoryLifecycleView *, u32);
void Shop_InitMessageBox(struct SceneInventoryLifecycleView *);
void Shop_ReadCompletionScript(struct SceneInventoryLifecycleView *);
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void Shop_RefreshCatalogDisplay(struct SceneInventoryLifecycleView *);
void Sound_PlaySong(u32);
void SceneInputPanel_RequestAction(struct SceneInventoryLifecycleView *,
                                   void (*)(struct SceneInventoryLifecycleView *));
void Shop_WaitForController(struct SceneInventoryLifecycleView *);
s32 Shop_InitializeController(struct SceneInventoryLifecycleView *owner)
{
    owner->messageText = 0;
    owner->equipmentText = 0;
    owner->unknown1A90 = 0;
    owner->itemText = 0;
    owner->consumableText = 0;
    owner->mode = 4;
    owner->unknown1AA5 = 0;
    owner->interactionFlags = 0;
    owner->unknown1AAC = 0;
    owner->unknown1AAD[0] = 0;
    owner->page = 0;
    owner->pageCount = 1;
    owner->itemPageCount = 0;
    switch (owner->presentation) {
        case 0:
            Shop_BuildUnlockedEquipmentCatalog(owner);
            owner->itemPageCount = owner->pageCount;
            break;
        case 2:
            Shop_BuildConsumableCatalog(owner);
            owner->itemPageCount = owner->pageCount;
            break;
        case 1:
        default:
            Shop_BuildUnlockedItemCatalog(owner);
            owner->itemPageCount = owner->pageCount;
            break;
        case 3:
        case 4:
            Shop_ReadScriptCatalog(owner);
            break;
    }
    Shop_InitCatalogBackground(owner, owner->presentation);
    Shop_InitPanel(owner, owner->presentation);
    Shop_InitMessageBox(owner);
    Shop_ReadCompletionScript(owner);
    ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xffff, 0);
    Shop_RefreshCatalogDisplay(owner);
    Sound_PlaySong(0x118);
    SceneInputPanel_RequestAction(owner, Shop_WaitForController);
    return 0;
}
