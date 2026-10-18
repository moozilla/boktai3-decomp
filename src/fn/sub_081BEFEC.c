#include "global.h"
#include "scene_input.h"
struct SceneInventoryCategoryMenu {
    u8 unknown0[0x258];
    struct SceneInputWidget prompt;
    u8 unknown2B8[0x1260];
    u8 choiceResource[0x235];
    u8 choiceState;
    u8 unknown174E[0x76];
    u8 choiceRepeat;
};
void Storage_HideItemDetails(struct SceneInventoryCategoryMenu *);
void Storage_BeginSelectedItemTransition(struct SceneInventoryCategoryMenu *);
void SceneInventory_BeginCategoryBrowse(struct SceneInventoryCategoryMenu *);
void sub_081BEF28(struct SceneInventoryCategoryMenu *);
void SceneInventory_SetState(struct SceneInventoryCategoryMenu *,
                             void (*)(struct SceneInventoryCategoryMenu *), u32);
void SceneInventory_UpdateCategoryConfirmation(struct SceneInventoryCategoryMenu *p)
{
    s32 result =
        SceneInput_TickChoice(&p->prompt, p->choiceResource, &p->choiceState, &p->choiceRepeat);
    if (result == 1) {
        Storage_HideItemDetails(p);
        Storage_BeginSelectedItemTransition(p);
        SceneInventory_SetState(p, sub_081BEF28, 0);
    } else if (result == 0) {
        Sound_PlaySong(0xDE);
        SceneInventory_BeginCategoryBrowse(p);
    }
}
