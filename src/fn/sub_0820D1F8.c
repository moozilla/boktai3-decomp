#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void Screamer_UpdateDamageParticles(void);
void Screamer_Destroy(void);
s32 Screamer_InitFromScript(void *, void *);
void *Screamer_Create(void *p)
{
    void *r = Task_Create(8, 0x220);
    if (r) {
        Task_SetCallbacks(r, Screamer_UpdateDamageParticles, Screamer_Destroy);
        if (Screamer_InitFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
