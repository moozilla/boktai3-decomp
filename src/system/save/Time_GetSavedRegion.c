// COMPILER: old_agbcc
#include "global.h"
#include "link/session/session_save.h"
/* Time_GetSavedRegion: Returns the saved region index used by Time_OnLoadSave */

u32 Time_GetSavedRegion(void)
{
    return ((struct SessionSave *)gSessionState)->region;
}
