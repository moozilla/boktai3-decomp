#include "monsters/cannons/plasma_cannon.h"
s32 PlasmaCannon_UpdateDamageState(struct OrangeSpineActor *);
s32 PlasmaCannon_OnTaskDestroy(u8 *);
s32 PlasmaCannon_InitializeFromScript(struct OrangeSpineActor *, void *);
void *PlasmaCannon_Create(void *p)
{
    struct OrangeSpineActor *r = Task_Create(8, sizeof(struct OrangeSpineActor));
    if (r) {
        Task_SetCallbacks(r, (void (*)(void *))PlasmaCannon_UpdateDamageState,
                          (void (*)(void *))PlasmaCannon_OnTaskDestroy);
        if (PlasmaCannon_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
