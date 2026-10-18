#define PlayerStatusPanel_RestoreStats sub_0816D40C
#include "global.h"
#include "player.h"
struct PlayerStatusPanel {
    u8 unknown0[0xA40];
    struct Player *player;
    u8 unknownA44[0x385C];
    u8 depleted;
    u8 unknown42A1[0x5C9];
    u16 savedStatA;
    u8 unknown486C[2];
    u16 savedStatB;
    u8 unknown4870[0xF];
    u8 active;
};
s32 Inventory_HasItem(s32);
void sub_0816D40C(struct PlayerStatusPanel *panel)
{
    panel->player->currentStatA = panel->savedStatA;
    panel->player->currentStatB = panel->savedStatB;
    if (panel->player->currentStatA == 0) {
        panel->depleted = 1;
        if (Inventory_HasItem(0x26))
            panel->active = 0;
    } else {
        panel->active = 0;
    }
}
