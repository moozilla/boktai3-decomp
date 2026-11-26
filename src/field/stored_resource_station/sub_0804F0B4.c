#include "field/stored_resource_station/resource_station.h"
#include "engine/shared/player.h"
void ResourceStation_TransferStoredResource(struct ResourceStation *station)
{
    u32 *playerSlot = (u32 *)&station->player;
    struct Player *player = (void *)*playerSlot;
    u8 *playerBytes = (void *)player;
    if (player->tetherMode == 8) {
        u32 m = 0x40;
        if (player->statusFlags & m) {
            s32 *balance = &gGameState->unk77C;
            s32 stored = *balance;
            if (stored > 0) {
                if (*(u16 *)(playerBytes + 0x428) < *(u16 *)(playerBytes + 0x42a)) {
                    *balance = stored - 1;
                    (*(u16 *)((u8 *)*playerSlot + 0x428))++;
                }
            }
        }
    } else {
        AnimData_SelectPaletteBank(station->graphics, 0x1ae);
        station->state = 0;
    }
    station->nearby = 1;
}
