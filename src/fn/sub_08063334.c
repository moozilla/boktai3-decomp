#include "scene_inventory_lifecycle.h"
void SceneInventory_BuildUnlockedEquipmentCatalog(struct SceneInventoryLifecycleView *);
void SceneInventory_BuildConsumableCatalog(struct SceneInventoryLifecycleView *);
void SceneInventory_BuildUnlockedItemCatalog(struct SceneInventoryLifecycleView *);
void SceneInventory_ReadScriptCatalog(struct SceneInventoryLifecycleView *);
void SceneInventory_InitCatalogBackground(struct SceneInventoryLifecycleView *, u32);
void SceneInventory_InitPanel(struct SceneInventoryLifecycleView *, u32);
void SceneInventory_InitMessageBox(struct SceneInventoryLifecycleView *);
void SceneInventory_ReadCompletionScript(struct SceneInventoryLifecycleView *);
void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);
void SceneInventory_RefreshCatalogDisplay(struct SceneInventoryLifecycleView *);
void Sound_PlaySong(u32);
void SceneInputPanel_RequestAction(struct SceneInventoryLifecycleView *,
                                   void (*)(struct SceneInventoryLifecycleView *));
void SceneInventory_WaitForController(struct SceneInventoryLifecycleView *);
s32 SceneInventory_InitializeController(struct SceneInventoryLifecycleView *owner)
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
            SceneInventory_BuildUnlockedEquipmentCatalog(owner);
            owner->itemPageCount = owner->pageCount;
            break;
        case 2:
            SceneInventory_BuildConsumableCatalog(owner);
            owner->itemPageCount = owner->pageCount;
            break;
        case 1:
        default:
            SceneInventory_BuildUnlockedItemCatalog(owner);
            owner->itemPageCount = owner->pageCount;
            break;
        case 3:
        case 4:
            SceneInventory_ReadScriptCatalog(owner);
            break;
    }
    SceneInventory_InitCatalogBackground(owner, owner->presentation);
    SceneInventory_InitPanel(owner, owner->presentation);
    SceneInventory_InitMessageBox(owner);
    SceneInventory_ReadCompletionScript(owner);
    sub_082279A8(2, 5, 4, 4, 4, 0xffff, 0);
    SceneInventory_RefreshCatalogDisplay(owner);
    Sound_PlaySong(0x118);
    SceneInputPanel_RequestAction(owner, SceneInventory_WaitForController);
    return 0;
}
