#include "global.h"
#define gHudManager    gUnk_02000214
struct HudManager;
extern struct HudManager *gUnk_02000214;
struct HudManager *Hud_GetManager(void)
{
    return gUnk_02000214;
}
