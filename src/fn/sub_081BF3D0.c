#include "global.h"
#include "scene_input.h"
#define SceneInventory_UpdateSortConfirmation sub_081BF3D0
#define SceneInventory_SortSelectedCategory sub_081BED9C
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
void sub_081BED9C(struct SceneInventoryCategoryMenu *);
void Storage_HideItemDetails(struct SceneInventoryCategoryMenu *);
void sub_081BFC14(struct SceneInventoryCategoryMenu *, s32);
void sub_081BF3D0(struct SceneInventoryCategoryMenu *p)
{
    s32 result = SceneInput_TickChoice(&p->prompt, p->choiceResource, &p->choiceState, &p->choiceRepeat);
    if (result == 1) {
        Sound_PlaySong(0xDD);
        sub_081BED9C(p);
        Storage_HideItemDetails(p);
        sub_081BFC14(p, p->returnState);
    } else if (result == 0) {
        Sound_PlaySong(0xDE);
        sub_081BFC14(p, p->returnState);
    }
}
