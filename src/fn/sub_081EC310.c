#include "global.h"
#include "ram.h"
struct P {
    u32 a;
    u32 b;
};
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u32 Script_GetPc(void);
void Results_SetFromScript(void)
{
    if (Script_SeekToKeyword(0x72))
        (*(struct P *)&gScreenDelayParameters).a = Script_GetPc();
    if (Script_SeekToKeyword(0x65))
        (*(struct P *)&gScreenDelayParameters).b = Script_GetValue();
}
