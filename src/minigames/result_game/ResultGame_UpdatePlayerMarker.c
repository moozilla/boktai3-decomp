#include "global.h"
#include "minigames/result_game/result_tutorial.h"
extern u16 gUnknown_03005260[];
extern int gUnk_0824DB38[];
extern u8 gUnk_0824DB64[], gUnk_0824DB6F[];
void ResultGame_UpdatePlayerMarker(struct ResultMenu *menu)
{
    u32 jump = gUnknown_03005260[1] & 1;
    if (jump) {
        menu->playerVelocity = gUnk_0824DB38[menu->level];
        Sound_PlaySong(0x3ec);
    } else if (menu->playerVelocity > 0) {
        menu->playerVelocity = 0;
        menu->accelerationDelay = 0;
    } else if (menu->accelerationDelay < gUnk_0824DB64[menu->level]) {
        menu->playerVelocity = 0;
        menu->accelerationDelay++;
    } else {
        menu->playerVelocity -= gUnk_0824DB6F[menu->level];
        if (menu->playerVelocity < -22)
            menu->playerVelocity = -22;
    }
    menu->playerPosition += menu->playerVelocity;
}
