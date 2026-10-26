#include "global.h"
#include "player.h"
#include "game_state.h"
struct PlayerPanel {
    u8 unknown[0xa40];
    struct Player *player;
};
u32 PlayerPanel_GetStatusValue(struct PlayerPanel *panel)
{
    s32 index;
    s32 ticks;
    u32 slot;
    if (panel->player->scriptStatus[0]) {
        index = 2;
        goto selected;
    }
    slot = panel->player->timedEventSlot;
    ticks = *(s32 *)((u8 *)gGameState + slot * 4 + 0x628);
    index = 0;
    if (ticks > 0)
        index = 6;
selected:
    return panel->player->menuStatusValues[index];
}
