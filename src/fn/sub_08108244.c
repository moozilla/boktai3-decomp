#include "global.h"
/* Provisional names and field interpretations; confidence low. */
#define Tile_SetEnabledFromScript sub_08108244
struct Tile {
    u32 flags;
    u8 pad4[0xBE - 4];
    u16 refresh;
    u16 flagsC0;
};
struct Tile *Pool1B8_FindByName(u32);
s32 Script_GetKeywordValue(u32, s32);
void sub_08108244(void)
{
    s32 name = Script_GetKeywordValue(0x69, -1);
    s32 enabled;
    struct Tile *tile;
    if (name >= 0) {
        enabled = Script_GetKeywordValue(0x73, 0);
        tile = Pool1B8_FindByName(name);
        if (tile) {
            if (enabled == 1) {
                tile->flagsC0 |= 0x200;
                tile->flags |= enabled;
            } else {
                tile->flagsC0 &= 0xFDFF;
                tile->refresh = 1;
            }
        }
    }
}
