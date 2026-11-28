#include "global.h"
#include "monsters/cannons/vulcan_cannon_worker.h"
void Script_SetPc(u8 *);
u8 *Script_GetPc(void);
s32 Script_GetValue(void);
void VulcanCannon_ReadScriptValueList(struct VulcanCannon *p)
{
    s32 i;

    Script_SetPc(p->delayScript);
    i = 0;
    if (Script_GetPc() != 0) {
        p->delayCount = Script_GetValue();
        if (p->delayCount > 8)
            p->delayCount = 8;
        if (p->delayCount != 0) {
            for (; i < p->delayCount; i++)
                p->delays[i] = Script_GetValue();
        }
    }
}
