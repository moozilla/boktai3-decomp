#include "hit_switch.h"
#define HitSwitch_AddFromScript sub_08054B64
u32 HitSwitch_AddFromScript(void)
{
    struct HitSwitchManager *manager = gUnk_02000490;
    if (manager == 0)
        manager = sub_08054AD0();
    return sub_08054834(manager);
}
