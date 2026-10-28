#include "global.h"
#include "engine.h"
#include "menu_parameters.h"
/* Parameters at 0x020003D0 (meaning unknown). */
void Params3D0_SetFromScript(void)
{
    struct TimeDisplayParameters *p = &gTimeDisplayParameters;
    p->M = 0;
    if (Script_SeekToKeyword('g'))
        p->g = Script_GetValue();
    if (Script_SeekToKeyword('o'))
        p->o = Script_GetValue();
    if (Script_SeekToKeyword('l'))
        p->l = Script_GetValue();
    if (Script_SeekToKeyword('s'))
        p->s = Script_GetValue();
    if (Script_SeekToKeyword('d'))
        p->d = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        p->c = Script_GetValue();
    if (Script_SeekToKeyword('t'))
        p->timeout = Script_GetValue();
    if (Script_SeekToKeyword('b'))
        p->silent = Script_GetValue();
    if (Script_SeekToKeyword('f')) {
        p->fadeOut = Script_GetValue();
        p->confirmFade = Script_GetValue();
    }
    if (Script_SeekToKeyword('M'))
        p->M = Script_GetValue();
}
