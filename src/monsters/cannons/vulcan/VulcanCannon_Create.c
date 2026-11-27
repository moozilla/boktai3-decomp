#include "monsters/cannons/vulcan_cannon_worker.h"
s32 VulcanCannon_UpdateDamageState(struct VulcanCannon *);
s32 VulcanCannon_Destroy(u8 *);
s32 VulcanCannon_InitializeFromScript(void *, void *);
void *VulcanCannon_Create(void *p)
{
    void *r = Task_Create(8, sizeof(struct VulcanCannon));
    if (r) {
        Task_SetCallbacks(r, (void (*)(void *))VulcanCannon_UpdateDamageState,
                          (void (*)(void *))VulcanCannon_Destroy);
        if (VulcanCannon_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
