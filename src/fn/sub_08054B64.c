// MATCHING_CONCESSION: register-pinning
#include "hit_switch.h"
u32 HitSwitch_AddFromScript(void)
{
    struct HitSwitchManager *manager = gUnk_02000490;
    if (manager == 0)
        manager = HitSwitch_CreateManager();
    HitSwitch_InitEntryFromScript(manager);
    {
        /* The registry dispatcher observes r0; no semantic return is established. */
        u32 result;
        asm("" : "=r"(result));
        return result;
    }
}
