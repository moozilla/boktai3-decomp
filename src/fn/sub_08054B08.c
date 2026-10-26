#include "hit_switch.h"
u32 HitSwitch_GetCompletionFromScript(void)
{
    struct HitSwitchManager *manager = gHitSwitchManager;
    if (manager) {
        s32 tag;
        s32 i;
        if (Script_SeekToKeyword('i'))
            tag = Script_GetValue();
        else
            tag = 0;
        for (i = 0; i < manager->count; i++)
            if (manager->entries[i].tag == tag)
                return manager->entries[i].completed;
    }
    return 0;
}
