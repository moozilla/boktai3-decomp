#include "global.h"
#include "engine.h"
#include "menu_parameters.h"
/* Parameters at 0x020002E8 (meaning unknown). */
void ScreenParams2E8_SetFromScript(void)
{
    struct MessageMenuParameters *p = &gMessageMenuParameters;
    p->c = 0;
    p->f = 0;
    if (Script_SeekToKeyword('l'))
        p->lBlock = Script_GetPc();
    if (Script_SeekToKeyword('o'))
        p->o = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        p->c = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        p->f = Script_GetValue();
}
