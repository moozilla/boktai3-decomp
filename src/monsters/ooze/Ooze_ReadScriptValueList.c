#include "monsters/ooze.h"
void Script_SetPc(u8 *);
u8 *Script_GetPc(void);
s32 Script_GetValue(void);
void Ooze_ReadScriptValueList(struct Ooze *p)
{
    s32 i;
    s16 *np;
    Script_SetPc(p->delayScript);
    i = 0;
    if (Script_GetPc() != 0) {
        s32 v = Script_GetValue();
        np = &p->delayCount;
        *np = v;
        if (*np > 8)
            *np = 8;
        if (*np != 0 && i < *np) {
            do {
                p->delays[i] = Script_GetValue();
                i++;
            } while (i < p->delayCount);
        }
    }
}
