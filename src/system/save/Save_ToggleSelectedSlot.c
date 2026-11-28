#include "global.h"
#include "link/session/session_save.h"
u8 Save_WriteHeader(void);
u8 Save_ToggleSelectedSlot(void)
{
    struct SessionSave *session = (void *)gSessionState;
    session->selectedSlot = 1 - session->selectedSlot;
    return Save_WriteHeader();
}
