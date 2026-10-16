#include "global.h"
#include "engine.h"
void sub_08177DEC(u32);
void Hud_SetModeFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        sub_08177DEC(Script_GetValue());
}
