#include "global.h"
struct P {
    u32 a;
    u32 b;
};
extern struct P gScreenDelayParameters;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u32 Script_GetPc(void);
void Results_SetFromScript(void)
{
    if (Script_SeekToKeyword(0x72))
        gScreenDelayParameters.a = Script_GetPc();
    if (Script_SeekToKeyword(0x65))
        gScreenDelayParameters.b = Script_GetValue();
}
