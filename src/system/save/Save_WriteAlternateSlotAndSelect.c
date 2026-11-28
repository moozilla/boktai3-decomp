#include "global.h"
#include "link/session/session_save.h"
u8 Save_WriteMainState(s32);
u8 Save_WriteContinuation(s32);
u8 Save_WriteHeader(void);
/* Switch to the other of two slots (0/1) if both checks accept it. */
u8 Save_WriteAlternateSlotAndSelect(void)
{
    s32 other = 1 - ((struct SessionSave *)gSessionState)->selectedSlot;
    if (Save_WriteMainState(other) && Save_WriteContinuation(other)) {
        ((struct SessionSave *)gSessionState)->selectedSlot = other;
        return Save_WriteHeader();
    }
    return 0;
}
