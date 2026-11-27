#include "global.h"
#include "link/session/session_save.h"
extern u8 *gGameState;
u8 Save_ReadHeader(void);
void Settings_LoadOrInitializeDefaults(void)
{
    if (gSessionState) {
        u8 r = Save_ReadHeader();
        if (r == 0) {
            u16 five = 5;
            s32 limit;
            struct SessionSave *session;
            *(u16 *)(gGameState + 0x12) = five;
            limit = 0xe6;
            session = (void *)gSessionState;
            session->solarBaseline = limit;
            session->selectedSlot = r;
            ((struct SessionSave *)gSessionState)->saveRequested = r;
            ((struct SessionSave *)gSessionState)->unknownA = r;
        }
    }
}
