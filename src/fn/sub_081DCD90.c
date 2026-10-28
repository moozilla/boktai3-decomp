#include "global.h"
#include "engine.h"
#include "menu_parameters.h"
s32 Script_GetValueSafe(void);
/* Parameters at 0x02000310 (meaning unknown). */
/* Script: -n block, -d (read with Script_GetValueSafe), -o, -c and -f
   (default 0). */
void ScreenParams310_SetFromScript(void)
{
    struct RegionMenuParameters *p = &gRegionMenuParameters;
    p->flag = 0;
    if (Script_SeekToKeyword('n'))
        p->nBlock = Script_GetPc();
    if (Script_SeekToKeyword('d'))
        p->table = Script_GetValueSafe();
    if (Script_SeekToKeyword('o'))
        p->confirmProc = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        p->cancelProc = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        p->flag = Script_GetValue();
}
