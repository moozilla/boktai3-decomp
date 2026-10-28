#include "global.h"
#include "menu_parameters.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Time_SetDateMenuFromScript(void)
{
    if (Script_SeekToKeyword(0x6f))
        gDateMenuParameters.confirmProc = Script_GetValue();
    if (Script_SeekToKeyword(0x63))
        gDateMenuParameters.cancelProc = Script_GetValue();
    if (Script_SeekToKeyword(0x66))
        gDateMenuParameters.flag = Script_GetValue();
}
