#include "global.h"
u16 Eeprom_WriteBlockWithFullDelay(u16, const u16 *), Eeprom_VerifyBlock(u16, const u16 *);

s32 Save_WriteBlock(u16 start, const u16 *data, s32 length)
{
    s32 blocks = length >> 3, i, address;
    const u16 *p;
    u16 header[4];

    p = data;
    address = start;
    for (i = 0; i < blocks; p += 4, address++, i++)
        if (Eeprom_WriteBlockWithFullDelay(address, p))
            goto fail;
    p = data;
    address = start;
    for (i = 0; i < blocks; p += 4, address++, i++)
        if (Eeprom_VerifyBlock(address, p))
            goto fail;
    header[0] = 0;
    header[1] = 0;
    for (i = 0; i < length >> 1; i++) {
        header[0] ^= data[i];
        header[1] += data[i];
    }
    header[2] = length;
    header[3] = 0xffff;
    if (Eeprom_WriteBlockWithFullDelay(address, header) || Eeprom_VerifyBlock(address, header))
        goto fail;
    length += 8;
    return length;
fail:
    return -1;
}
