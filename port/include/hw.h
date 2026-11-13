#ifndef BOKTAI_PORT_HW_H
#define BOKTAI_PORT_HW_H

#include <stddef.h>
#include <stdint.h>

/* Hardware is independent of the host UI and renderer. Addresses passed to the
 * bus are GBA addresses; pointers passed to BIOS routines are host pointers. */
#define HW_CPU_HZ 16777216u
#define HW_CYCLES_PER_LINE 1232u
#define HW_LINES_PER_FRAME 228u
#define HW_CYCLES_PER_FRAME (HW_CYCLES_PER_LINE * HW_LINES_PER_FRAME)
#define HW_IO_BASE 0x04000000u
#define HW_SAVE_MAX 0x20000u

enum HwResult {
    HW_OK = 0,
    HW_TRUNCATED = -1,
    HW_BAD_DATA = -2,
    HW_NO_SPACE = -3,
    HW_UNSUPPORTED = -4
};

/* Bounded variants are suitable for untrusted runtime ROM assets. The SDK
 * wrappers below retain the original trusted-pointer API. SWI numbers select
 * LZ77 (0x11/12), Huffman (0x13), RL (0x14/15), or delta (0x16/17/18). */
int hw_bios_decompress(unsigned swi, const void *src, size_t src_size,
                       void *dst, size_t dst_size, size_t *written);
int32_t hw_bios_div(int32_t numerator, int32_t denominator, int32_t *remainder);
uint16_t hw_bios_sqrt(uint32_t value);
int16_t hw_bios_arctan(int16_t tangent);
uint16_t hw_bios_arctan2(int16_t x, int16_t y);

struct BgAffineSrcData;
struct BgAffineDstData;
struct ObjAffineSrcData;
void CpuSet(const void *src, void *dst, uint32_t control);
void CpuFastSet(const void *src, void *dst, uint32_t control);
int32_t Div(int32_t numerator, int32_t denominator);
int32_t Mod(int32_t numerator, int32_t denominator);
int32_t DivArmMod(int32_t denominator, int32_t numerator);
uint16_t Sqrt(uint32_t value);
int16_t ArcTan(int16_t tangent);
uint16_t ArcTan2(int16_t x, int16_t y);
void BgAffineSet(struct BgAffineSrcData *src, struct BgAffineDstData *dst, int32_t count);
void ObjAffineSet(struct ObjAffineSrcData *src, void *dst, int32_t count, int32_t stride);
void LZ77UnCompWram(const uint32_t *src, void *dst);
void LZ77UnCompVram(const uint32_t *src, void *dst);
void RLUnCompWram(const uint32_t *src, void *dst);
void RLUnCompVram(const uint32_t *src, void *dst);
void HuffUnComp(const uint32_t *src, void *dst);
void Diff8bitUnFilterWram(const uint32_t *src, void *dst);
void Diff8bitUnFilterVram(const uint32_t *src, void *dst);
void Diff16bitUnFilter(const uint32_t *src, void *dst);
void RegisterRamReset(uint32_t flags);
void SoftReset(uint32_t flags);
void VBlankIntrWait(void);
void SoundBiasReset(void);
void SoundBiasSet(void);

typedef struct Hw Hw;
typedef struct HwHost {
    void *user;
    /* Return a mapped range, or NULL for an unmapped address. Must not return
     * transient storage. The callback is also usable with wasm address 0. */
    uint8_t *(*map)(void *user, uint32_t address, size_t size);
    void (*interrupt)(void *user, uint16_t pending);
    void (*wait)(void *user, uint16_t interrupt_mask, int discard_old);
    void (*reset)(void *user);
    int64_t (*unix_time)(void *user);
    void (*save_dirty)(void *user);
    void (*bios_error)(void *user, unsigned swi, int result);
} HwHost;

typedef struct HwDma {
    uint32_t source, destination, initial_destination, remaining;
    uint16_t control;
    uint8_t enabled;
} HwDma;

typedef struct HwTimer {
    uint16_t reload, counter, control;
    uint32_t fraction;
} HwTimer;

enum HwSaveKind { HW_SAVE_EEPROM_512, HW_SAVE_EEPROM_8K, HW_SAVE_FLASH_64K, HW_SAVE_FLASH_128K };

struct Hw {
    HwHost host;
    HwDma dma[4];
    HwTimer timers[4];
    uint8_t io[0x400];
    uint8_t save[HW_SAVE_MAX];
    size_t save_size;
    enum HwSaveKind save_kind;
    uint8_t save_dirty, flash_state, flash_id, flash_bank;
    uint16_t keys;
    uint8_t key_irq_match;
    uint64_t cycles;
    uint16_t scanline;
    uint16_t gpio_data, gpio_direction, gpio_enable;
    uint16_t solar_threshold, solar_clocks;
    uint8_t solar_output;
    uint8_t rtc_command, rtc_bits, rtc_data[8], rtc_count, rtc_read;
    uint8_t rtc_status, rtc_active;
    int64_t rtc_offset;
    uint16_t eeprom_bits, eeprom_address, eeprom_read_bit;
    uint8_t eeprom_command, eeprom_buffer[8];
    int8_t fifo[2][32], fifo_sample[2];
    uint8_t fifo_read[2], fifo_count[2];
};

void hw_init(Hw *hw, const HwHost *host);
void hw_bind(Hw *hw); /* bind SDK BIOS/save/RTC wrappers to this instance */
Hw *hw_bound(void);
uint8_t hw_read8(Hw *hw, uint32_t address);
uint16_t hw_read16(Hw *hw, uint32_t address);
uint32_t hw_read32(Hw *hw, uint32_t address);
void hw_write8(Hw *hw, uint32_t address, uint8_t value);
void hw_write16(Hw *hw, uint32_t address, uint16_t value);
void hw_write32(Hw *hw, uint32_t address, uint32_t value);
/* Import ordinary stores to the mapped I/O page. Explicit bus writes are
 * required for write-one-to-clear flags, FIFO/trigger writes and polling. */
void hw_sync_io(Hw *hw);
void hw_step_cycles(Hw *hw, uint32_t cycles);
void hw_hblank(Hw *hw, unsigned scanline);
void hw_vblank(Hw *hw);
void hw_set_keys(Hw *hw, uint16_t pressed);
void hw_request_interrupt(Hw *hw, uint16_t mask);
void hw_set_solar(Hw *hw, unsigned threshold); /* 0 bright .. 255 dark, not lux */
void hw_rtc_datetime(Hw *hw, uint8_t bcd[7]); /* UTC unless host applies an offset */
int hw_rtc_set_datetime(Hw *hw, const uint8_t bcd[7]);
void hw_save_configure(Hw *hw, enum HwSaveKind kind);
int hw_save_load(Hw *hw, const void *bytes, size_t size);
const uint8_t *hw_save_data(const Hw *hw, size_t *size);
void hw_save_clean(Hw *hw);
int hw_bios_call(unsigned swi, uintptr_t registers[4]);
void hw_system_call(unsigned swi); /* argument-free inline SDK calls */

#endif
