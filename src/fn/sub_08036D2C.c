#include "global.h"
u16 *WirelessText_GetCharacterTable(void);
u16 WirelessText_GetCharacterTableByte(u8);
u8 WirelessText_FindCharacterIndex(u32);
void WirelessText_EncodeString(const u8 *source, u8 *destination, int count)
{
    const u8 **alias;
    int i = 0;
    while (i < count) {
        u32 first = *source;
        u32 code;
        if (!first) {
            break;
        }
        first <<= 24;
        alias = &source;
        if (((s32)first) >= 0) {
            code = *(source++);
        } else {
            {
                int high;
                code = source[0];
                high = (*alias)[1] << 8;
                code |= high;
            }
            source += 2;
        }
        destination[i] = WirelessText_FindCharacterIndex(code);
        i++;
    }

    while (i < count) {
        destination[i++] = 0;
    }

    destination[count] = 0;
}