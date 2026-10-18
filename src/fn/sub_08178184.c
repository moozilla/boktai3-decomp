#include "global.h"

#include "engine.h"
extern u8 *gUnk_02000214;
void Hud_SetField13F1FromScript(void)
{
    if (gUnk_02000214 && Script_SeekToKeyword('f'))
        gUnk_02000214[0x13F1] = Script_GetValue();
}
