// COMPILER: old_agbcc
#include "global.h"
#include "engine/runtime.h"
/* Time_GetSavedRegion: Returns the saved region index used by Time_OnLoadSave */

u32 Time_GetSavedRegion(void)
{
    return *(u32 *)(gSessionState + 24);
}
