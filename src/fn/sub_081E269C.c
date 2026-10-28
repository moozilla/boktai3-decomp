#include "global.h"
#include "menu_parameters.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u32 Script_GetPc(void);
void SoundTest_ConfigureFromScript(void)
{
    if (Script_SeekToKeyword(0x72))
        gSoundTestParameters.text = Script_GetPc();
    if (Script_SeekToKeyword(0x63))
        gSoundTestParameters.confirmProc = Script_GetValue();
}
