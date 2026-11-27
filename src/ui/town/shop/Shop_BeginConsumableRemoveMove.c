#include "ui/town/shop_actions.h"
#include "system/game_state/game_state.h"
struct ShopOwnedConsumables {
    u8 unknown0[0x160];
    s16 id;
};
void Shop_BeginConsumableRemoveMove(struct ShopActionView *panel)
{
    struct SceneInputPosition starts[2], targets[2];
    u32 enabled[2], frames[2], attributes[2];
    struct ShopActionFlagsView *display = (void *)panel;
    struct SceneInputPosition *target;
    enabled[0] = 1;
    display->widgets[panel->savedSelection + 5].flags |= 1;
    {
        u32 address = panel->savedSelection;
        struct GameState *game;
        game = gGameState;
        address <<= 1;
        address += (u32)game;
        frames[0] = ((struct ShopOwnedConsumables *)address)->id + 0x121;
    }
    attributes[0] = 0x373;
    SceneInput_GetGridPosition(starts, panel->savedSelection);
    target = targets;
    SceneInput_GetGridPosition(target, 24);
    enabled[1] = 0;
    frames[1] = 0x121;
    attributes[1] = 0x373;
    {
        struct SceneInputPosition *start = starts;
        start[1].x = 0;
        starts[1].y = 0;
        starts[1].z = 0;
    }
    target[1].x = 0;
    target[1].y = 0;
    target[1].z = 0;
    SceneInput_BeginPairTransition(&((struct ShopTransitionView *)panel)->pair, enabled,
                                   starts, target, frames, attributes, 12);
    Sound_PlaySong(0x38A);
}
