#include "global.h"
#ifdef NONMATCHING
// fuzzy: 91.7%
#include "hit_switch.h"
u32 HitSwitch_AddFromScript(void)
{
    struct HitSwitchManager *manager = gUnk_02000490;
    if (manager == 0)
        manager = HitSwitch_CreateManager();
    HitSwitch_InitEntryFromScript(manager);
    /* The original forwards the void callee's incidental return-address word.
     * This draft provides an explicit result; ASM preserves the original ABI. */
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", HitSwitch_AddFromScript);
#endif
