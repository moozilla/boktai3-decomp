#include "global.h"
#include "engine.h"
#include "menu_parameters.h"
/* Parameters at 0x020002A0 (meaning unknown). */
void ScreenParams2A0_SetFromScript(void)
{
    if (Script_SeekToKeyword('g'))
        gOptionMenuParameters.callbacks.keyword.g = Script_GetValue();
    if (Script_SeekToKeyword('o'))
        gOptionMenuParameters.callbacks.keyword.o = Script_GetValue();
    if (Script_SeekToKeyword('s'))
        gOptionMenuParameters.callbacks.keyword.s = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gOptionMenuParameters.callbacks.keyword.c = Script_GetValue();
    if (Script_SeekToKeyword('r'))
        gOptionMenuParameters.callbacks.keyword.r = Script_GetValue();
    if (Script_SeekToKeyword('l'))
        gOptionMenuParameters.callbacks.keyword.l = Script_GetValue();
    if (Script_SeekToKeyword('d'))
        gOptionMenuParameters.callbacks.keyword.d = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        gOptionMenuParameters.callbacks.keyword.f = Script_GetValue();
}
