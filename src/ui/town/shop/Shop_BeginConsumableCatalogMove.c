#include "ui/town/shop_actions.h"
#include "system/game_state/game_state.h"
void Shop_BeginConsumableCatalogMove(struct ShopActionView *panel)
{
    struct SceneInputPosition starts[2], targets[2];
    u32 enabled[2], frames[2], attributes[2];
    enabled[0] = 1;
    frames[0] = panel->catalog[panel->savedSelection - 16 + panel->page * 8] + 0x121;
    attributes[0] = 0x373;
    SceneInput_GetGridPosition(&starts[0], panel->savedSelection);
    SceneInput_GetGridPosition(&targets[0], panel->selection);
    enabled[1] = 0;
    frames[1] = 0x151;
    attributes[1] = 0x373;
    starts[1].x = 0;
    starts[1].y = 0;
    starts[1].z = 0;
    targets[1].x = 0;
    targets[1].y = 0;
    targets[1].z = 0;
    SceneInput_BeginPairTransition(&((struct ShopTransitionView *)panel)->pair, enabled, starts,
                                   targets, frames, attributes, 12);
    Sound_PlaySong(0x38A);
}
