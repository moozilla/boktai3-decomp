#include "monsters/dragon_puppy.h"
void Script_SetPc(u8 *);
u8 *Script_GetPc(void);
s32 Script_GetValue(void);
void DragonPuppy_ReadAttackSchedule(struct DragonPuppy *p)
{
    s32 i;

    Script_SetPc((u8 *)p->attackSchedule);
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
