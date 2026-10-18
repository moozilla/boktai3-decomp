#include "global.h"
#include "scene_input.h"
#define SceneInventory_UpdateCategoryConfirmation sub_081BEFEC
#define SceneInventory_BeginCategoryBrowse sub_081BEE48
#define SceneInventory_SetState sub_081BDB34
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
void sub_081BEE48(struct SceneInventoryCategoryMenu *);
void sub_081BEF28(struct SceneInventoryCategoryMenu *);
void sub_081BDB34(struct SceneInventoryCategoryMenu *, void (*)(struct SceneInventoryCategoryMenu *), u32);
void sub_081BEFEC(struct SceneInventoryCategoryMenu *p)
{
    s32 result = SceneInput_TickChoice(&p->prompt, p->choiceResource, &p->choiceState, &p->choiceRepeat);
    if (result == 1) {
        Storage_HideItemDetails(p);
        Storage_BeginSelectedItemTransition(p);
        sub_081BDB34(p, sub_081BEF28, 0);
    } else if (result == 0) {
        Sound_PlaySong(0xDE);
        sub_081BEE48(p);
    }
}
