#include "resource_station.h"
#define ResourceStation_UpdateAnimation sub_0804F140
void ResourceStation_UpdateAnimation(struct ResourceStation *station)
{
    struct ResourceStationPlayer *player = station->player;
    u32 mask = 64;
    if (player->flags & mask) {
        switch ((s32)((station->animationFrame >> 1) & 3)) {
            case 1:
                AnimData_SelectTiles(station->graphics, 0x1B0);
                break;
            case 0:
            case 2:
                AnimData_SelectTiles(station->graphics, 0x1AF);
                break;
            case 3:
                AnimData_SelectTiles(station->graphics, 0x1AE);
                break;
        }
        station->animationFrame++;
    } else {
        u8 *frame = &station->animationFrame;
        if (*frame > 4)
            *frame = 4;
        {
            u32 value = *frame;
            if (value) {
                AnimData_SelectTiles(station->graphics, 0x1AF);
                (*frame)--;
            } else {
                AnimData_SelectTiles(station->graphics, 0x1AE);
                *frame = value;
            }
        }
    }
}
