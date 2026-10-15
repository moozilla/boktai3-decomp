#include "scene_inventory_tail.h"
struct SceneInventorySwapView {
    u8 unknown0[0x1AF0];
    struct SceneInputPair pair;
};
s32 SceneInput_TickPairTransition(struct SceneInputPair *);
void sub_0822D94C(u32, u32);
void sub_0822D080(u32, u32);
void sub_0822E138(u32, u32);
s32 SceneInventory_TickSwapTransition(struct SceneInventoryTail *p)
{
    SceneInputPanel_ResetHighlights(&p->display, &p->highlight);
    if (SceneInput_TickPairTransition(&((struct SceneInventorySwapView *)p)->pair)) {
        switch (p->category) {
            case 0:
                sub_0822D94C(p->grid.savedSelection, p->grid.selection);
                break;
            case 1:
                sub_0822D080(p->grid.savedSelection, p->grid.selection);
                break;
            case 2:
                sub_0822E138(p->grid.savedSelection, p->grid.selection);
                break;
        }
        SceneInventory_BeginThirdSelection(p);
    }
    return 0;
}
