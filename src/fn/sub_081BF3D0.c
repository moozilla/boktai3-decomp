#include "global.h"
#include "scene_input.h"
struct SceneInventoryCategoryMenu {
    u8 unknown0[0x258];
    struct SceneInputWidget prompt;
    u8 unknown2B8[0x1260];
    u8 choiceResource[0x235];
    u8 choiceState;
    u8 unknown174E[2];
    s32 returnState;
    u8 unknown1754[0x70];
    u8 choiceRepeat;
};
void SceneInventory_SortSelectedCategory(struct SceneInventoryCategoryMenu *);
void Storage_HideItemDetails(struct SceneInventoryCategoryMenu *);
void Storage_BeginModeSelection(struct SceneInventoryCategoryMenu *, s32);
void SceneInventory_UpdateSortConfirmation(struct SceneInventoryCategoryMenu *p)
{
    s32 result =
        SceneInput_TickChoice(&p->prompt, p->choiceResource, &p->choiceState, &p->choiceRepeat);
    if (result == 1) {
        Sound_PlaySong(0xDD);
        SceneInventory_SortSelectedCategory(p);
        Storage_HideItemDetails(p);
        Storage_BeginModeSelection(p, p->returnState);
    } else if (result == 0) {
        Sound_PlaySong(0xDE);
        Storage_BeginModeSelection(p, p->returnState);
    }
}
