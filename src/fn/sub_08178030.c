#include "global.h"

struct Hud {
    u8 pad[0xA5E];
    u8 mode, savedMode;
};
extern struct Hud *gUnk_02000214;
/* Copies the HUD mode (+0xA5E) to +0xA5F. */
void Hud_SaveMode(void)
{
    struct Hud *hud = gUnk_02000214;
    if (hud)
        hud->savedMode = hud->mode;
}
