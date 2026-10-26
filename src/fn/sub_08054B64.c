#include "global.h"

struct HitSwitchManager;
extern struct HitSwitchManager *gHitSwitchManager;
struct HitSwitchManager *HitSwitch_CreateManager(void);
/* Declared with a result here: the original forwards whatever r0 holds when
   this (really void) routine returns. */
s32 HitSwitch_InitEntryFromScript(struct HitSwitchManager *);

s32 HitSwitch_AddFromScript(void)
{
    struct HitSwitchManager *manager = gHitSwitchManager;
    if (manager == 0)
        manager = HitSwitch_CreateManager();
    return HitSwitch_InitEntryFromScript(manager);
}
