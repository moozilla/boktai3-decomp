#include "global.h"
#include "ram.h"
#include "runtime.h"
/* Solar_ResetState: Clears the solar task pointer and current and cached intensity levels */
#define Solar_ResetState sub_0822C110
extern u8 *gGameState;
void sub_0822C110(void)
{
    gSolarController = 0;
    *(u16 *)(gGameState + 0x874) = 0;
    *(u16 *)(gGameState + 0x876) = 0;
    gSolarSavedLevel = 0;
    gSolarSavedCachedLevel = 0;
    gSolarPaused = 0;
}
