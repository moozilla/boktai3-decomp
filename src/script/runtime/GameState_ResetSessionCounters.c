#include "link/session/session_save.h"
void GameState_ResetSessionCounters(void)
{
    struct SessionSave *p = (void *)gSessionState;
    if (p) {
        p->points = 0;
        p->magic = 0;
        p->counterA = 0;
        p->counterB = 0;
    }
}
