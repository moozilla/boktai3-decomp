#include "monsters/statue.h"
u32 Statue_UpdateDamageState(struct Statue *);
s32 Statue_Destroy(u8 *);
s32 Statue_InitializeFromScript(void *, void *);
void *Statue_Create(void *p)
{
    void *r = Task_Create(8, sizeof(struct Statue));
    if (r) {
        Task_SetCallbacks(r, (void (*)(void *))Statue_UpdateDamageState,
                          (void (*)(void *))Statue_Destroy);
        if (Statue_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
