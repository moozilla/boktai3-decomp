#include "global.h"
#include "link/session/session_save.h"
extern u8 *gGameState;
u8 Save_ReadHeader(void);
void Memory_Clear(void *, s32);
s32 GameState_LoadSessionOrInitDefaults(void)
{
    u8 result = Save_ReadHeader();
    if (!result) {
        if (gSessionState)
            Memory_Clear(gSessionState, sizeof(struct SessionSave));
        {
            u32 value = 5;
            *(u16 *)(gGameState + 0x12) = value;
        }
        {
            u32 value = 230;
            ((struct SessionSave *)gSessionState)->solarBaseline = value;
        }
        ((struct SessionSave *)gSessionState)->selectedSlot = result;
        ((struct SessionSave *)gSessionState)->saveRequested = result;
        ((struct SessionSave *)gSessionState)->unknownA = result;
    }
    return result;
}
