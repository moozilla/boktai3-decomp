#include "global.h"
u16 *WirelessText_GetCharacterTable(void);
u16 WirelessText_GetCharacterTableByte(u8);
u8 WirelessText_FindCharacterIndex(u16);
void WirelessText_DecodeString(const u8 *source, u8 *destination, int count)
{
    int i = 0;
    while (i < count) {
        u16 code = WirelessText_GetCharacterTableByte(source[i]);
        if (code <= 127) {
            *destination++ = code;
        } else {
            destination[0] = code;
            destination[1] = code >> 8;
            destination += 2;
        }
        i++;
    }
    *destination = 0;
}