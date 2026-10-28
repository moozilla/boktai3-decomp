// COMPILER: old_agbcc
#include "global.h"
#include "runtime.h"
/* Time_GetSavedRegion: Returns the saved region index used by Time_OnLoadSave */
#define Time_GetSavedRegion sub_082281C0

u32 sub_082281C0(void)
{
    return *(u32 *)(gSessionState + 24);
}
