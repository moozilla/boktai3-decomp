#include "monsters/axe_armor/axe_armor.h"
void Script_SetPc(u8 *);
u8 *Script_GetPc(void);
s32 Script_GetValue(void);
void AxeArmor_ReadScriptValueList(struct AxeArmor *p)
{
    s32 i;

    Script_SetPc(p->attackSchedule);
    i = 0;
    if (Script_GetPc() != 0) {
        p->attackCount = Script_GetValue();
        if (p->attackCount > 8)
            p->attackCount = 8;
        if (p->attackCount != 0) {
            for (; i < p->attackCount; i++)
                p->attackTimes[i] = Script_GetValue();
        }
    }
}
