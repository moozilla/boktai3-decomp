#include "global.h"
u16 Eeprom_ReadBlock(u16, u16 *);

s32 Save_ReadBlock(u16 address, u16 *data, s32 length)
{
    s32 blocks = length >> 3;
    u16 *orig = data;
    s32 i;
    u16 err;
    u16 header[4];
    u16 check[2];

    for (i = 0; i < blocks; i++) {
        if (Eeprom_ReadBlock(address, data))
            goto fail;
        data += 4;
        address++;
    }
    err = Eeprom_ReadBlock(address, header);
    if (err)
        goto fail;
    check[0] = 0;
    check[1] = 0;
    for (i = 0; i < length >> 1; i++) {
        check[0] ^= orig[i];
        check[1] += orig[i];
    }
    check[0] ^= header[0];
    if (err || check[0] || check[1] != header[1] || length != header[2]) {
    fail:
        return -1;
    }
    return 0;
}
