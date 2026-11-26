#include "global.h"
#include "ui/widgets/scene_input.h"
struct StorageCategoryMenu {
    u8 unknown0[0x258];
    struct AnimSprite prompt;
    u8 unknown2B8[0x1260];
    u8 choiceResource[0x235];
    u8 choiceState;
    u8 unknown174E[0x76];
    u8 choiceRepeat;
};
void Storage_HideItemDetails(struct StorageCategoryMenu *);
void Storage_BeginSelectedItemTransition(struct StorageCategoryMenu *);
void Storage_BeginCategoryBrowse(struct StorageCategoryMenu *);
void Storage_CompleteCategoryRemoval(struct StorageCategoryMenu *);
void Storage_SetState(struct StorageCategoryMenu *,
                             void (*)(struct StorageCategoryMenu *), u32);
void Storage_UpdateCategoryConfirmation(struct StorageCategoryMenu *p)
{
    s32 result =
        SceneInput_TickChoice(&p->prompt, p->choiceResource, &p->choiceState, &p->choiceRepeat);
    if (result == 1) {
        Storage_HideItemDetails(p);
        Storage_BeginSelectedItemTransition(p);
        Storage_SetState(p, Storage_CompleteCategoryRemoval, 0);
    } else if (result == 0) {
        Sound_PlaySong(0xDE);
        Storage_BeginCategoryBrowse(p);
    }
}
