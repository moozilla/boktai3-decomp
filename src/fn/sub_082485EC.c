#include "global.h"
#include "ram.h"
extern const u8 gUnk_08602D2C[], gUnk_08602D38[];
u32 Eeprom_SetCapacity(u16 x)
{
    u32 r = 0;
    if (x == 4)
        gEEPROMConfig = gUnk_08602D2C;
    else if (x == 0x40)
        gEEPROMConfig = gUnk_08602D38;
    else {
        gEEPROMConfig = gUnk_08602D2C;
        r = 1;
    }
    return r;
}
