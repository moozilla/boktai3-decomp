#include "global.h"
#include "ram.h"

struct Hud {
    u8 pad[0xA5E];
    u8 mode, savedMode;
};
/* Copies the HUD mode (+0xA5E) to +0xA5F. */
void Hud_SaveMode(void)
{
    struct Hud *hud = gFieldHud;
    if (hud)
        hud->savedMode = hud->mode;
}
