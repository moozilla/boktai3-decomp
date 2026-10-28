#include "global.h"
#include "engine.h"
#include "menu_parameters.h"
/* Parameters at 0x020003A0 (meaning unknown). */
void ScreenParams3A0_SetFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        gListMenuParameters.mBlock = Script_GetPc();
    if (Script_SeekToKeyword('a'))
        gListMenuParameters.aBlock = Script_GetPc();
    if (Script_SeekToKeyword('l'))
        gListMenuParameters.l = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        gListMenuParameters.f = Script_GetValue();
    if (Script_SeekToKeyword('F'))
        gListMenuParameters.F = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gListMenuParameters.c = Script_GetValue();
    if (Script_SeekToKeyword('b'))
        gListMenuParameters.b = Script_GetValue();
}
