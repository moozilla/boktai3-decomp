// CFLAGS: -O1 -mthumb-interwork
#include "global.h"
/* Comparative source: laqieer/libgbabackup at 9597360b36d22df1ead5dd59566a3c8618cff6c0, src/eeprom.c (tmc lineage). */
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
u16 sub_08248764(u16,const u16 *);
u16 sub_08248930(u16 address, const u16 *data)
{
    u8 i;
    u16 result;
    for (i = 0; i < 3; i++) {
        result = sub_08248764(address, data);
        if (result == 0) {
            result = sub_082488D8(address, data);
            if (result == 0) break;
        }
    }
    return result;
}
