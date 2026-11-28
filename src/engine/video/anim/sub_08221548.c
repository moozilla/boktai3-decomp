#include "global.h"
/* CRC-16 (MSB first, init 0xFFFF, final complement) over data[start..end). */
u32 sub_08221548(u8 *data, u32 start, u32 end, u32 poly)
{
    u32 crc = 0xffff;
    u32 bit;
    u32 i;
    for (i = start; i < end; i++) {
        crc ^= data[i] << 8;
        for (bit = 0; bit < 8; bit++) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ poly;
            else
                crc <<= 1;
        }
    }
    return ~crc & 0xffff;
}
