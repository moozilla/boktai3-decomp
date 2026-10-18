#include "global.h"

u16 *WirelessText_GetCharacterTable(void);
u32 WirelessText_GetCharacterTableByte(u8 i)
{
    return WirelessText_GetCharacterTable()[i];
}
