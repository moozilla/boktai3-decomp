// CFLAGS: -O1 -mthumb-interwork
#include "global.h"
/* Adapted from zeldaret/tmc 6fb6dfb4a7efbe24d0fd1dda5097af6131faacde, src/eeprom.c.
 * Initial comparison: laqieer/libgbabackup at 9597360b36d22df1ead5dd59566a3c8618cff6c0, src/eeprom.c (tmc lineage). */
typedef struct EEPROMConfig { u32 unk_00; u16 size, waitcnt; u8 address_width; } EEPROMConfig;
extern const EEPROMConfig *gUnk_03006A9C;
#define gEEPROMConfig gUnk_03006A9C
#define EEPROM_OUT_OF_RANGE 0x80ff
#define EEPROM_COMPARE_FAILED 0x8000
#define ARRAY_COUNT(a) (sizeof(a)/sizeof((a)[0]))
#define REG_IME (*(vu16 *)0x04000208)
#define REG_WAITCNT (*(vu16 *)0x04000204)
#define REG_DMA3SAD (*(vu32 *)0x040000D4)
#define REG_DMA3DAD (*(vu32 *)0x040000D8)
#define REG_DMA3CNT (*(vu32 *)0x040000DC)
#define REG_DMA3CNT_H (*(vu16 *)0x040000DE)
#define REG_VCOUNT (*(vu16 *)0x04000006)
#define REG_EEPROM (*(u16 *)0x0d000000)
void sub_08248634(const void *,void *,u16);
u16 sub_082486B4(u16,u16 *);
u16 sub_08248778(u16,const u16 *,u8);
u16 sub_082488D8(u16,const u16 *);
u16 sub_082486B4(u16 address, u16* data) {
    u16 buffer[0x44];

    u16* ptr;
    u8 t1, t2;
    u16 value;

    if (address >= gEEPROMConfig->size) {
        return EEPROM_OUT_OF_RANGE;
    } else {
        ptr = buffer;
        // setup address
        (u8*)ptr += (gEEPROMConfig->address_width << 1) + 1;
        ((u8*)ptr)++;
        for (t1 = 0; t1 < gEEPROMConfig->address_width; t1++) {
            *(ptr--) = address;
            address >>= 1;
        }
        // read request
        *(ptr--) = 1;
        *ptr = 1;
        // send address to eeprom
        sub_08248634(buffer, (u16*)0xd000000, gEEPROMConfig->address_width + 3);
        // recieve data
        sub_08248634((u16*)0xd000000, buffer, 0x44);
        // 4 bit junk
        ptr = buffer + 4;
        data += 3;
        // copy data into output buffer
        for (t1 = 0; t1 < 4; t1++) {
            value = 0;
            for (t2 = 0; t2 < 0x10; t2++) {
                value <<= 1;
                value |= (*ptr++) & 1;
            }
            *(data--) = value;
        }
        return 0;
    }
}
