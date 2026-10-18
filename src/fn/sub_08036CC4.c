#include "global.h"
u16 *WirelessText_GetCharacterTable(void);
u16 WirelessText_GetCharacterTableByte(u8);
int WirelessText_FindCharacterIndex(u16);
int WirelessText_FindCharacterIndex(u16 code)
{
    u16 *base = WirelessText_GetCharacterTable();
    u16 *table;
    int i = 0;
    int result;
    table = base;
    do {
        if (*table == code) {
            {
                u32 narrowed = i << 24;
                result = narrowed >> 24;
            }
            goto done;
        }
        table++;
    } while (++i <= 255);
    result = 0;
done:
    return result;
}