#include "global.h"
#include "runtime.h"
extern u8 *gGameState;
u8 Save_ReadHeader(void);
void Settings_LoadOrInitializeDefaults(void)
{
    if (gSessionState) {
        u8 r = Save_ReadHeader();
        if (r == 0) {
            u16 five = 5;
            s32 limit;
            u8 *p;
            *(u16 *)(gGameState + 0x12) = five;
            limit = 0xe6;
            p = gSessionState;
            *(s32 *)(p + 4) = limit;
            p[8] = r;
            gSessionState[9] = r;
            gSessionState[10] = r;
        }
    }
}
