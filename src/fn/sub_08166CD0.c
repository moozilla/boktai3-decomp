#include "global.h"
#include "player.h"
struct PlayerPanel {
    u8 unknown0[0xa40];
    struct Player *player;
    u8 unknownA44[0x3e6c];
    u16 snapshotValues[3];
};
void PlayerPanel_SnapshotValues(struct PlayerPanel *panel)
{
    s32 i;
    for (i = 0; i < 3; i++)
        panel->snapshotValues[i] = panel->player->menuSnapshotValues[i];
}
