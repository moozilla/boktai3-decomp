#include "link/session/session_save.h"
void GameState_ClearSessionFlags(void)
{
    if (gSessionState) {
        ((struct SessionSave *)gSessionState)->saveRequested = 0;
        ((struct SessionSave *)gSessionState)->daylightSaving = 0;
        ((struct SessionSave *)gSessionState)->unknownA = 0;
    }
}
