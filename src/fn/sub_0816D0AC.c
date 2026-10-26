#define Player_ClearScriptStatus Player_ClearStateTimer
#include "global.h"
#include "player.h"
#include "game_state.h"
struct PlayerStatusPanel {
    u8 unknown0[0xA40];
    struct Player *player;
    u8 unknownA44[0x3E22];
    u16 resetState;
};
void Player_ClearScriptStatus(struct Player *, s32);
void PlayerStatusPanel_ResetScriptCounters(struct PlayerStatusPanel *panel, s32 mask)
{
    struct Player *player = panel->player;
    s32 i;
    if (player) {
        panel->resetState = 0;
        for (i = 0; i <= 1; i++) {
            if ((mask >> i) & 1)
                Player_ClearScriptStatus(player, i);
        }
        if (mask & 8) {
            u32 index = player->timedEventSlot;
            struct GameState *state = gGameState;
            u32 address = index * 4 + (u32)state;
            *(u32 *)(address + 0x628) = 0;
        }
    }
}
