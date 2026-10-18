#include "global.h"
u32 Eeprom_WriteBlock(u32, u32, u32);
u16 Eeprom_WriteBlockWithFullDelay(u16 a, u32 b)
{
    return Eeprom_WriteBlock(a, b, 1);
}
