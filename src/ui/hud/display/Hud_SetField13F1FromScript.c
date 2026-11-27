#include "global.h"

#include "engine/engine.h"
#include "engine/ram.h"
void Hud_SetField13F1FromScript(void)
{
    if (gFieldHud && Script_SeekToKeyword('f'))
        gFieldHud[0x13F1] = Script_GetValue();
}
