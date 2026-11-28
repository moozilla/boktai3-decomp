#include "global.h"
#include "engine/ram.h"
#include "system/save/region_list.h"
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
u32 Region_InitFromScript(struct RegionList *list)
{
    if (Script_SeekToKeyword('m')) {
        s32 *region = list->regions;

        while (Script_GetPc())
            *region++ = Script_GetValue();
    }
    gRegionList = list;
    return 0;
}
