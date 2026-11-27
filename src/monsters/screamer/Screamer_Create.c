#include "monsters/screamer/screamer_state.h"
s32 Screamer_UpdateDamageParticles(struct ScreamerState *);
s32 Screamer_Destroy(u8 *);
s32 Screamer_InitFromScript(void *, void *);
void *Screamer_Create(void *p)
{
    void *r = Task_Create(8, sizeof(struct ScreamerState));
    if (r) {
        Task_SetCallbacks(r, (void (*)(void *))Screamer_UpdateDamageParticles,
                          (void (*)(void *))Screamer_Destroy);
        if (Screamer_InitFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
