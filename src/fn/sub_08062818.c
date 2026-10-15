#include "scene_inventory_tail.h"
#define SceneInventory_TickDiscardTransition sub_08062818
struct SceneInventorySwapView {
    u8 unknown0[0x1AF0];
    struct SceneInputPair pair;
};
s32 SceneInput_TickPairTransition(struct SceneInputPair *);
void sub_0822D918(u32);
s32 sub_0806288C(struct SceneInventoryTail *);
void sub_0822CE18(u32);
void sub_0822E11C(u32);
s32 SceneInventory_TickDiscardTransition(struct SceneInventoryTail *p)
{
    SceneInputPanel_ResetHighlights(&p->display, &p->highlight);
    if (SceneInput_TickPairTransition(&((struct SceneInventorySwapView *)p)->pair)) {
        u32 selection = p->grid.savedSelection;
        switch (p->category) {
            case 0:
                sub_0822D918(selection);
                break;
            case 1:
                sub_0822CE18(selection);
                break;
            case 2:
                sub_0822E11C(selection);
                break;
        }
        sub_0806130C(p, (SceneInventoryCallback)sub_0806288C);
    }
    return 0;
}
