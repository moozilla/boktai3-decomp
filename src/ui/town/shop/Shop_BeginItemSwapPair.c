#include "ui/town/shop_actions.h"
#include "system/game_state/game_state.h"
u32 Selection_GetDisplayFrame(s32);
void Shop_BeginItemSwapPair(struct ShopActionView *panel)
{
    struct SceneInputPosition starts[2], targets[2];
    u32 enabled[2], frames[2], attributes[2];
    s32 i;
    for (i = 0; i < 2; i++) {
        u32 slot;
        s32 id;
        if (i == 0)
            slot = panel->savedSelection;
        else
            slot = panel->selection;
        ((struct ShopActionFlagsView *)panel)->widgets[slot + 5].flags |= 1;
        id = Inventory_GetItemId(slot);
        if (id >= 0)
            enabled[i] = 1;
        else
            enabled[i] = 0;
        frames[i] = Selection_GetDisplayFrame(id);
        attributes[i] = 0x373;
        SceneInput_GetGridPosition(&starts[i], slot);
    }
    for (i = 0; i < 2; i++)
        targets[i] = starts[1 - i];
    SceneInput_BeginPairTransition(&((struct ShopTransitionView *)panel)->pair, enabled, starts,
                                   targets, frames, attributes, 12);
    Sound_PlaySong(0x38A);
}
