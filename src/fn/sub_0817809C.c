#include "global.h"
#include "engine.h"
void Hud_SetMode(u32);
void Hud_SetModeFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        Hud_SetMode(Script_GetValue());
}
