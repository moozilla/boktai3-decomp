#include "monsters/ooze.h"
void Script_SetPc(u8 *);
u8 *Script_GetPc(void);
s32 Script_GetValue(void);
void Ooze_ReadScriptValueList(struct Ooze *ooze)
{
    s32 i;

    Script_SetPc(ooze->delayScript);
    i = 0;
    if (Script_GetPc() != 0) {
        ooze->delayCount = Script_GetValue();
        if (ooze->delayCount > 8)
            ooze->delayCount = 8;
        if (ooze->delayCount != 0) {
            for (; i < ooze->delayCount; i++)
                ooze->delays[i] = Script_GetValue();
        }
    }
}
