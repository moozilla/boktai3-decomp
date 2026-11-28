#include "monsters/cannons/wind_cannon.h"
void Script_SetPc(u8 *);
u8 *Script_GetPc(void);
s32 Script_GetValue(void);
void WindCannon_ReadToggleSchedule(struct FanCannon *p)
{
    s32 i;

    Script_SetPc((u8 *)p->schedule);
    i = 0;
    if (Script_GetPc() != 0) {
        p->scheduleCount = Script_GetValue();
        if (p->scheduleCount > 8)
            p->scheduleCount = 8;
        if (p->scheduleCount != 0) {
            for (; i < p->scheduleCount; i++)
                p->times[i] = Script_GetValue();
        }
    }
}
