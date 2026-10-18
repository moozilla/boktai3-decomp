#include "global.h"
struct SceneInventoryCategoryMenu {
    u8 unknown0[0xF3A];
    s16 introY;
    u8 unknownF3C[0x812];
    u16 introFrame;
};
void SceneInventory_BeginCategoryBrowse(struct SceneInventoryCategoryMenu *);
void SceneInventory_UpdateBrowseIntro(struct SceneInventoryCategoryMenu *p)
{
    p->introFrame++;
    if (p->introFrame <= 11) {
        if ((p->introFrame >> 1) & 1)
            p->introY = 104;
        else
            p->introY = 104 - ((20 - p->introFrame) >> 3);
    } else if (p->introFrame <= 15) {
        p->introY = 104;
    } else {
        SceneInventory_BeginCategoryBrowse(p);
    }
}
