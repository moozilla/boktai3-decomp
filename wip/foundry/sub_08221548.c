#define Checksum_ComputeCrc16 sub_08221548
#include "global.h"
u32 sub_08221548(const u8 *data, u32 start, u32 end, u32 polynomial)
{
    u32 crc = 0xFFFF;
    while (start < end) {
        u32 bit;
        crc ^= data[start] << 8;
        bit = 0;
        start++;
        do {
            if (crc & 0x8000)
                crc = (crc << 1) ^ polynomial;
            else
                crc <<= 1;
            bit++;
        } while (bit < 8);
    }
    return ~crc & 0xFFFF;
}
