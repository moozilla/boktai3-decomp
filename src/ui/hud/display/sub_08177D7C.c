#include "global.h"
#include "engine/ram.h"
#define gHudManager gFieldHud
struct HudManager;
struct HudManager *Hud_GetManager(void)
{
    return gFieldHud;
}
