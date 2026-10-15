#include "scene_inventory_lifecycle.h"
#define SceneInventory_InitializeController sub_08063334
void sub_08062DEC(struct SceneInventoryLifecycleView *);
void sub_080630FC(struct SceneInventoryLifecycleView *);
void sub_08062F10(struct SceneInventoryLifecycleView *);
void sub_0806300C(struct SceneInventoryLifecycleView *);
void sub_08062BF4(struct SceneInventoryLifecycleView *, u32);
void sub_08062C08(struct SceneInventoryLifecycleView *, u32);
void sub_08062DA8(struct SceneInventoryLifecycleView *);
void sub_08063194(struct SceneInventoryLifecycleView *);
void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);
void sub_080628D4(struct SceneInventoryLifecycleView *);
void Sound_PlaySong(u32);
void SceneInputPanel_RequestAction(struct SceneInventoryLifecycleView *,
                                   void (*)(struct SceneInventoryLifecycleView *));
void sub_080631C4(struct SceneInventoryLifecycleView *);
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
    owner->savedPageCount = 0;
    switch (owner->presentation) {
        case 0:
            sub_08062DEC(owner);
            owner->savedPageCount = owner->pageCount;
            break;
        case 2:
            sub_080630FC(owner);
            owner->savedPageCount = owner->pageCount;
            break;
        case 1:
        default:
            sub_08062F10(owner);
            owner->savedPageCount = owner->pageCount;
            break;
        case 3:
        case 4:
            sub_0806300C(owner);
            break;
    }
    sub_08062BF4(owner, owner->presentation);
    sub_08062C08(owner, owner->presentation);
    sub_08062DA8(owner);
    sub_08063194(owner);
    sub_082279A8(2, 5, 4, 4, 4, 0xffff, 0);
    sub_080628D4(owner);
    Sound_PlaySong(0x118);
    SceneInputPanel_RequestAction(owner, sub_080631C4);
    return 0;
}
