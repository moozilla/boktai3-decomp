#include "monsters/axe_armor/axe_armor.h"
void Script_SetPc(u8 *);
u8 *Script_GetPc(void);
s32 Script_GetValue(void);
void AxeArmor_ReadScriptValueList(struct AxeArmor *p)
{
    s32 i;
    s16 *d;
    s16 *np;
    Script_SetPc(p->attackSchedule);
    i = 0;
    if (Script_GetPc() != 0) {
        s32 v = Script_GetValue();
        np = &p->attackCount;
        *np = v;
        if (*np > 8)
            *np = 8;
        if (*np != 0 && i < *np) {
            d = p->attackTimes;
            do {
                *d = Script_GetValue();
                d++;
                i++;
            } while (i < p->attackCount);
        }
    }
}
