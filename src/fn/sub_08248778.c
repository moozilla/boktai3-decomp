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
#define REG_EEPROM (*(vu16 *)0x0d000000)
void sub_08248634(const void *,void *,u16);
u16 sub_082486B4(u16,u16 *);
u16 sub_08248778(u16,const u16 *,u8);
u16 sub_082488D8(u16,const u16 *);
typedef unsigned long uintptr_t;
u16 sub_08248778(u16 address, const u16* data, u8 unk_3) {
    u16 buffer[0x52]; // this is one too large?
    vu16 timeout_flag;
    vu16 prev_vcount;      // stack + a6
    vu16 current_vcount;   // stack + a8
    vu32 passed_scanlines; // stack + ac
    u16 ret;
    vu16* temp2;

    u32 r2;

    u8 i, j;
    u16* ptr;

    if (address >= gEEPROMConfig->size)
        return EEPROM_OUT_OF_RANGE;

    ptr = (u16*)(0x42 + (uintptr_t)&buffer + (uintptr_t)(gEEPROMConfig->address_width * 2) + 0x42);
    *ptr-- = 0;
    // copy data into buffer
    for (i = 0; i < 4; i++) {
        r2 = *data++;
        for (j = 0; j < 16; j++) {
            *ptr = r2;
            ptr--;
            r2 = r2 >> 1;
        }
    }

    // copy address to buffer
    for (i = 0; i < gEEPROMConfig->address_width; i++) {
        *ptr = address;
        ptr--;
        address = address >> 1;
    }
    *ptr-- = 0;
    *ptr-- = 1;
    sub_08248634(buffer, (u16*)0xd000000, gEEPROMConfig->address_width + 0x43);
    ret = 0;
    timeout_flag = 0;
    prev_vcount = REG_VCOUNT;
    passed_scanlines = 0;

    while (1) {
        if (!timeout_flag) {
            if (REG_EEPROM & 1) {
                timeout_flag++;
                if (!unk_3)
                    break;
            }
        }

        current_vcount = REG_VCOUNT;
        if (current_vcount != prev_vcount) {
            if (current_vcount > prev_vcount) {
                passed_scanlines += (current_vcount - prev_vcount);
            } else {
                passed_scanlines += (current_vcount - (prev_vcount - 0xE4));
            }

            if (passed_scanlines > 0x88) {
                if (timeout_flag)
                    break;
                if ((REG_EEPROM & 1)) {
                    break;
                }

                ret = 0xc001;
                break;
            }
            prev_vcount = current_vcount;
        }
    }

    return ret;
}
