#include "global.h"
#include "ram.h"
#define gHudManager gFieldHud
struct HudManager;
struct HudManager *Hud_GetManager(void)
{
    return gFieldHud;
}
