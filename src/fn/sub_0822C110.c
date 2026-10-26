#include "global.h"
#include "ram.h"
/* Solar_ResetState: Clears the solar task pointer and current and cached intensity levels */
#define Solar_ResetState sub_0822C110
extern u8 *gGameState;
extern u16 gUnk_030054C0, gUnk_030054C4, gUnk_030054BC;
void sub_0822C110(void)
{
    gSolarController = 0;
    *(u16 *)(gGameState + 0x874) = 0;
    *(u16 *)(gGameState + 0x876) = 0;
    gUnk_030054C0 = 0;
    gUnk_030054C4 = 0;
    gUnk_030054BC = 0;
}
