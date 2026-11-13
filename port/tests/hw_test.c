#include "hw.h"
#include "gba/types.h"
#include <limits.h>

#define CHECK(condition) do { if (!(condition)) return __LINE__; } while (0)

static Hw machine;
static uint8_t ewram[0x40000], iwram[0x8000], io[0x400], palette[0x400], vram[0x18000], oam[0x400];
static unsigned interrupts, waits, resets;
static int64_t clock_seconds = INT64_C(1709251199); /* 2024-02-29 23:59:59 UTC */

static uint8_t *map(void *user, uint32_t address, size_t size)
{
    uint8_t *p = NULL;
    size_t length = 0;
    uint32_t base = address & 0xff000000;
    (void)user;
    switch (base) {
    case 0x02000000: p = ewram; length = sizeof(ewram); break;
    case 0x03000000: p = iwram; length = sizeof(iwram); break;
    case 0x04000000: p = io; length = sizeof(io); break;
    case 0x05000000: p = palette; length = sizeof(palette); break;
    case 0x06000000: p = vram; length = sizeof(vram); break;
    case 0x07000000: p = oam; length = sizeof(oam); break;
    }
    address -= base;
    return p && address <= length && size <= length - address ? p + address : NULL;
}

static void interrupt(void *user, uint16_t pending) { (void)user; interrupts |= pending; }
static void wait_irq(void *user, uint16_t mask, int discard) { (void)user; waits += mask + discard; }
static void reset(void *user) { (void)user; ++resets; }
static int64_t now(void *user) { (void)user; return clock_seconds; }

static int bios_tests(void)
{
    uint32_t src[16], dst[16];
    unsigned i;
    int32_t rem;
    uint8_t result[64];
    size_t written;
    const uint8_t lz[] = {0x10, 12, 0, 0, 0x10, 'A', 'B', 'C', 0x60, 2};
    const uint8_t rl[] = {0x30, 8, 0, 0, 1, 'a', 'b', 0x83, 'c'};
    const uint8_t huff[] = {0x28, 4, 0, 0, 1, 0xc0, 'A', 'B', 0, 0, 0, 0x60};
    const uint8_t huff4[] = {0x24, 2, 0, 0, 1, 0xc0, 3, 12, 0, 0, 0, 0x60};
    const uint8_t delta[] = {0x81, 4, 0, 0, 250, 10, 2, 255};
    const uint8_t delta16[] = {0x82, 4, 0, 0, 0xff, 0xff, 2, 0};
    struct BgAffineSrcData affine_src = {10000, 20000, 10, 20, 256, 512, 0x4000};
    struct BgAffineDstData affine_dst;
    struct ObjAffineSrcData object_src = {256, 512, 0};
    int16_t object_dst[16];
    for (i = 0; i < 16; ++i) { src[i] = i * 0x1020304; dst[i] = 0; }
    CpuSet(src, dst, 0x04000004);
    CHECK(dst[3] == src[3] && dst[4] == 0);
    CpuFastSet(src + 1, dst, 0x01000001);
    CHECK(dst[0] == src[1] && dst[7] == src[1] && dst[8] == 0);
    CHECK(hw_bios_div(-11, 3, &rem) == -3 && rem == -2);
    CHECK(hw_bios_div(INT_MIN, -1, &rem) == INT_MIN && rem == 0);
    CHECK(hw_bios_div(5, 0, &rem) == -1 && rem == 5);
    CHECK(Sqrt(0) == 0 && Sqrt(15) == 3 && Sqrt(UINT_MAX) == 65535);
    CHECK(ArcTan(0) == 0 && ArcTan(16384) == 8192 && ArcTan(-16384) == -8192);
    CHECK(ArcTan2(1, 0) == 0 && ArcTan2(0, 1) == 0x4000 && ArcTan2(-1, 0) == 0x8000 && ArcTan2(0, -1) == 0xc000);
    CHECK(ArcTan2(1, 1) == 0x2000 && ArcTan2(-1, -1) == 0xa000);
    CHECK(hw_bios_decompress(0x11, lz, sizeof(lz), result, sizeof(result), &written) == 0 && written == 12);
    for (i = 0; i < 12; ++i) CHECK(result[i] == "ABC"[i % 3]);
    CHECK(hw_bios_decompress(0x11, lz, sizeof(lz) - 1, result, sizeof(result), NULL) == HW_TRUNCATED);
    CHECK(hw_bios_decompress(0x11, lz, sizeof(lz), result, 11, NULL) == HW_NO_SPACE);
    CHECK(hw_bios_decompress(0x14, rl, sizeof(rl), result, sizeof(result), &written) == 0 && written == 8);
    CHECK(result[0] == 'a' && result[1] == 'b' && result[7] == 'c');
    CHECK(hw_bios_decompress(0x13, huff, sizeof(huff), result, sizeof(result), NULL) == 0);
    CHECK(result[0] == 'A' && result[1] == 'B' && result[2] == 'B' && result[3] == 'A');
    CHECK(hw_bios_decompress(0x13, huff4, sizeof(huff4), result, sizeof(result), NULL) == 0);
    CHECK(result[0] == 0xc3 && result[1] == 0x3c);
    CHECK(hw_bios_decompress(0x16, delta, sizeof(delta), result, sizeof(result), NULL) == 0);
    CHECK(result[0] == 250 && result[1] == 4 && result[2] == 6 && result[3] == 5);
    CHECK(hw_bios_decompress(0x18, delta16, sizeof(delta16), result, sizeof(result), NULL) == 0);
    CHECK(result[0] == 255 && result[1] == 255 && result[2] == 1 && result[3] == 0);
    BgAffineSet(&affine_src, &affine_dst, 1);
    CHECK(affine_dst.pa == 0 && affine_dst.pb == -256 && affine_dst.pc == 512 && affine_dst.pd == 0);
    CHECK(affine_dst.dx == 15120 && affine_dst.dy == 14880);
    for (i = 0; i < 16; ++i) object_dst[i] = 99;
    ObjAffineSet(&object_src, object_dst, 1, 8);
    CHECK(object_dst[0] == 256 && object_dst[4] == 0 && object_dst[8] == 0 && object_dst[12] == 512 && object_dst[1] == 99);
    return 0;
}

static void dma(unsigned channel, uint32_t source, uint32_t destination, uint16_t count, uint16_t control)
{
    uint32_t address = HW_IO_BASE + 0xb0 + channel * 12;
    hw_write32(&machine, address, source);
    hw_write32(&machine, address + 4, destination);
    hw_write32(&machine, address + 8, (uint32_t)control << 16 | count);
}

static int device_tests(void)
{
    unsigned i;
    Hw *hw = &machine;
    hw_write16(hw, HW_IO_BASE + 0x200, 0xffff);
    hw_write16(hw, HW_IO_BASE + 0x208, 1);
    hw_write16(hw, HW_IO_BASE + 0x132, 0xc003);
    hw_set_keys(hw, 1);
    CHECK(hw_read16(hw, HW_IO_BASE + 0x130) == 0x3fe && !(interrupts & 0x1000));
    hw_set_keys(hw, 3);
    CHECK(interrupts & 0x1000);
    hw_write16(hw, HW_IO_BASE + 0x202, 0x1000);
    CHECK(!(hw_read16(hw, HW_IO_BASE + 0x202) & 0x1000));
    for (i = 0; i < 64; ++i) hw_write8(hw, 0x02000000 + i, (uint8_t)i);
    dma(3, 0x02000000, 0x06000000, 8, 0xc400);
    CHECK(hw_read32(hw, 0x0600001c) == 0x1f1e1d1c && !(hw_read16(hw, HW_IO_BASE + 0xde) & 0x8000));
    CHECK(interrupts & 0x800);
    dma(3, 0x02000000, 0x06000040, 2, 0x9260); /* VBlank, repeat, reload dest */
    CHECK(hw_read16(hw, 0x06000040) == 0);
    hw_vblank(hw);
    CHECK(hw_read16(hw, 0x06000040) == 0x0100);
    hw_vblank(hw);
    CHECK(hw_read16(hw, 0x06000040) == 0x0504 && hw->dma[3].enabled);
    hw_write16(hw, HW_IO_BASE + 0xde, 0);
    dma(0, 0x02000000, 0x06000080, 1, 0xa000);
    hw_hblank(hw, 160);
    CHECK(hw->dma[0].enabled);
    hw_hblank(hw, 2);
    CHECK(hw_read16(hw, 0x06000080) == 0x0100 && !hw->dma[0].enabled);
    hw_write32(hw, HW_IO_BASE + 0x104, 0x00c4fffe); /* Timer1 cascades */
    hw_write32(hw, HW_IO_BASE + 0x100, 0x00c0fffc);
    hw_step_cycles(hw, 9);
    CHECK(hw->timers[0].counter == 0xfffd && hw->timers[1].counter == 0xfffe);
    CHECK((interrupts & 0x18) == 0x18);
    hw_write16(hw, HW_IO_BASE + 0x102, 0);
    hw_write16(hw, HW_IO_BASE + 0x106, 0);
    hw_write16(hw, HW_IO_BASE + 0x82, 0x0800);
    dma(1, 0x02000000, 0x040000a0, 1, 0xb640);
    hw_write32(hw, HW_IO_BASE + 0x100, 0x0080ffff);
    hw_step_cycles(hw, 2);
    CHECK(hw->fifo_sample[0] == 0 && hw->fifo_count[0] == 31 && hw->dma[1].source == 0x02000020);
    hw_write16(hw, HW_IO_BASE + 0x102, 0);
    hw_write16(hw, HW_IO_BASE + 0xc6, 0);
    hw_write8(hw, 0x05000001, 0x37);
    CHECK(hw_read16(hw, 0x05000000) == 0x3737);
    hw_write8(hw, 0x07000000, 0xff);
    CHECK(hw_read16(hw, 0x07000000) == 0);
    VBlankIntrWait(); CHECK(waits == 2);
    hw_system_call(3); CHECK(waits == 2 + 0x3080);
    iwram[0x7e00] = 0x55; iwram[0] = 0xff;
    RegisterRamReset(2); CHECK(iwram[0] == 0 && iwram[0x7e00] == 0x55);
    SoftReset(0); CHECK(resets == 1);
    return 0;
}

static void flash_command(uint8_t command)
{
    hw_write8(&machine, 0x0e005555, 0xaa);
    hw_write8(&machine, 0x0e002aaa, 0x55);
    hw_write8(&machine, 0x0e005555, command);
}

static int cartridge_tests(void)
{
    Hw *hw = &machine;
    uint8_t date[7], set[] = {0x24, 0x03, 0x01, 5, 0x12, 0x34, 0x56};
    uint16_t values[] = {0x1234,0x5678,0x9abc,0xdef0}, result[4];
    unsigned i;
    extern uint16_t Eeprom_WriteBlock(uint16_t, const uint16_t *, uint8_t);
    extern uint16_t Eeprom_ReadBlock(uint16_t, uint16_t *);
    extern uint16_t Eeprom_VerifyBlock(uint16_t, const uint16_t *);
    CHECK(Eeprom_WriteBlock(17, values, 1) == 0);
    CHECK(Eeprom_ReadBlock(17, result) == 0 && Eeprom_VerifyBlock(17, values) == 0);
    for (i = 0; i < 4; ++i) CHECK(result[i] == values[i]);
    CHECK(Eeprom_ReadBlock(1024, result) == 0x80ff);
    CHECK(hw->save_dirty && hw->save[17 * 8] == 0xde);
    hw_save_clean(hw); CHECK(!hw->save_dirty);
    /* Send EEPROM read command for block 17, then compare all wire bits. */
    hw_write16(hw, 0x0d000000, 1); hw_write16(hw, 0x0d000000, 1);
    for (i = 14; i; --i) hw_write16(hw, 0x0d000000, (17 >> (i - 1)) & 1);
    hw_write16(hw, 0x0d000000, 0);
    for (i = 0; i < 4; ++i) CHECK(hw_read16(hw, 0x0d000000) == 0);
    for (i = 0; i < 64; ++i) CHECK(hw_read16(hw, 0x0d000000) == ((hw->save[17 * 8 + i / 8] >> (7 - i % 8)) & 1));
    CHECK(hw_read16(hw, 0x0d000000) == 1);
    hw_save_configure(hw, HW_SAVE_FLASH_128K);
    flash_command(0x90); CHECK(hw_read8(hw, 0x0e000000) == 0xc2 && hw_read8(hw, 0x0e000001) == 9);
    hw_write8(hw, 0x0e000000, 0xf0);
    flash_command(0xa0); hw_write8(hw, 0x0e000123, 0x5a);
    CHECK(hw_read8(hw, 0x0e000123) == 0x5a);
    flash_command(0xb0); hw_write8(hw, 0x0e000000, 1);
    CHECK(hw_read8(hw, 0x0e000123) == 0xff);
    flash_command(0xb0); hw_write8(hw, 0x0e000000, 0);
    flash_command(0x80); hw_write8(hw, 0x0e005555, 0xaa); hw_write8(hw, 0x0e002aaa, 0x55); hw_write8(hw, 0x0e000123, 0x30);
    CHECK(hw_read8(hw, 0x0e000123) == 0xff);
    hw_rtc_datetime(hw, date);
    CHECK(date[0] == 0x24 && date[1] == 2 && date[2] == 0x29 && date[4] == 0x23 && date[6] == 0x59);
    ++clock_seconds; hw_rtc_datetime(hw, date);
    CHECK(date[1] == 3 && date[2] == 1 && date[4] == 0 && date[6] == 0);
    CHECK(hw_rtc_set_datetime(hw, set) == 0);
    hw_rtc_datetime(hw, date);
    CHECK(date[4] == 0x12 && date[5] == 0x34 && date[6] == 0x56);
    set[1] = 2; set[2] = 0x30;
    CHECK(hw_rtc_set_datetime(hw, set) == HW_BAD_DATA);
    hw_set_solar(hw, 3);
    hw_write16(hw, 0x080000c8, 1);
    hw_write16(hw, 0x080000c6, 7);
    hw_write16(hw, 0x080000c4, 2);
    for (i = 0; i < 3; ++i) {
        hw_write16(hw, 0x080000c4, 0);
        CHECK(!(hw_read16(hw, 0x080000c4) & 8));
        hw_write16(hw, 0x080000c4, 1);
    }
    CHECK(hw_read16(hw, 0x080000c4) & 8);
    return 0;
}

int hw_test_run(void)
{
    HwHost host = {0};
    int result;
    interrupts = waits = resets = 0;
    host.map = map; host.interrupt = interrupt; host.wait = wait_irq; host.reset = reset; host.unix_time = now;
    hw_init(&machine, &host); hw_bind(&machine);
    result = bios_tests(); if (result) return result;
    result = device_tests(); if (result) return result;
    return cartridge_tests();
}
