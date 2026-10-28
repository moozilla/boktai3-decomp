#include "global.h"
#include "engine.h"
#include "menu_parameters.h"
/* Parameters at 0x02000358 (meaning unknown). */
void ScreenParams358_SetFromScript(void)
{
    struct SelectionMenuParameters *p = &gSelectionMenuParameters;
    p->S = 0;
    if (Script_SeekToKeyword('p'))
        p->pBlock = Script_GetPc();
    if (Script_SeekToKeyword('P'))
        p->PBlock = Script_GetPc();
    if (Script_SeekToKeyword('N'))
        p->NBlock = Script_GetPc();
    if (Script_SeekToKeyword('s'))
        p->s = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        p->f = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        p->c = Script_GetValue();
    if (Script_SeekToKeyword('S'))
        p->S = 1;
}
