// MATCHING_CONCESSION: register-pinning
#include "hit_switch.h"
#define HitSwitch_AddFromScript sub_08054B64
u32 HitSwitch_AddFromScript(void)
{
    struct HitSwitchManager *manager = gUnk_02000490;
    if (manager == 0)
        manager = sub_08054AD0();
    sub_08054834(manager);
    {
        /* The registry dispatcher observes r0; no semantic return is established. */
        u32 result;
        asm("" : "=r"(result));
        return result;
    }
}
