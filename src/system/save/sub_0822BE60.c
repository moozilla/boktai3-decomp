#include "global.h"
#include "link/session/session_save.h"
u8 Save_ReadContinuation(u8);

u8 sub_0822BE60(void)
{
    return Save_ReadContinuation(((struct SessionSave *)gSessionState)->selectedSlot);
}
