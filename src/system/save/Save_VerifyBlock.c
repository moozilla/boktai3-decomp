#include "global.h"
u16 Eeprom_ReadBlock(u16, void *);
s32 Save_VerifyBlock(u16 start, s32 length)
{
    u16 address = start;
    s32 blocks = length >> 3, i;
    u16 header[4];
    u16 calc[2];
    u16 buf[4];
    calc[0] = 0;
    calc[1] = 0;
    for (i = 0; i < blocks; address++, i++) {
        s32 k;
        if (Eeprom_ReadBlock(address, buf))
            goto fail;
        for (k = 0; k <= 3; k++) {
            calc[0] ^= buf[k];
            calc[1] += buf[k];
        }
    }
    {
        u16 r = Eeprom_ReadBlock(address, header);
        calc[0] ^= header[0];
        if (r == 0 && calc[0] == 0 && calc[1] == header[1] && length == header[2])
            goto ok;
    }
fail:
    return -1;
ok:
    return 0;
}
