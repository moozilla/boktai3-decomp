#include "global.h"
#include "link/session/session_save.h"
u8 Save_ReadMainState(u32);
u8 Save_ReadContinuation(u32);
u32 Save_LoadAlternateSlot(void)
{
    u32 slot = 1 - ((struct SessionSave *)gSessionState)->selectedSlot;
    if (Save_ReadMainState(slot)) {
        if (Save_ReadContinuation(slot)) {
            ((struct SessionSave *)gSessionState)->selectedSlot = slot;
            return 1;
        }
    }
    return 0;
}
