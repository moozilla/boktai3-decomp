// COMPILER: old_agbcc
#include "global.h"
/* Time_GetSavedRegion: Returns the saved region index used by Time_OnLoadSave */
#define Time_GetSavedRegion sub_082281C0

extern u8 *gUnk_030053F8;

u32 sub_082281C0(void)
{
    return *(u32 *)(gUnk_030053F8 + 24);
}
