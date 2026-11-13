#include "hw.h"
#include "hw_rtc.h"
#include "gba/types.h"
#include "gba/m4a_internal.h"
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
    static uint32_t maximum_copy[0x200001];
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
    int16_t object_dst[32];
    for (i = 0; i < 16; ++i) { src[i] = i * 0x1020304; dst[i] = 0; }
    CpuSet(src, dst, 0x04000004);
    CHECK(dst[3] == src[3] && dst[4] == 0);
    CpuFastSet(src + 1, dst, 0x01000001);
    CHECK(dst[0] == src[1] && dst[7] == src[1] && dst[8] == 0);
    for (i = 0; i < 16; ++i) dst[i] = i;
    CpuFastSet(dst, dst + 1, 1);
    CHECK(dst[0] == 0 && dst[1] == 0 && dst[8] == 7 && dst[9] == 9);
    src[0] = 0x76543210;
    maximum_copy[0x200000] = 0x13579bdf;
    CpuFastSet(src, maximum_copy, 0x011fffff);
    for (i = 0; i < 0x200000; ++i) CHECK(maximum_copy[i] == 0x76543210);
    CHECK(maximum_copy[0x200000] == 0x13579bdf);
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
    {
        struct ObjInput { struct ObjAffineSrcData transform; uint16_t padding; } inputs[2] = {
            {{256,512,0},0xaaaa}, {{128,256,0x4000},0xbbbb}
        };
        for (i = 0; i < 32; ++i) object_dst[i] = 99;
        ObjAffineSet(&inputs[0].transform, object_dst, 2, 8);
        CHECK(object_dst[0] == 256 && object_dst[12] == 512 && object_dst[16] == 0);
        CHECK(object_dst[20] == -128 && object_dst[24] == 256 && object_dst[28] == 0 && object_dst[31] == 99);
    }
    {
        uint8_t input = 0xe4, info[] = {1,0,2,8,1,0,0,0};
        uint32_t unpacked = 0xffffffff;
        uintptr_t registers[] = {(uintptr_t)&input, (uintptr_t)&unpacked, (uintptr_t)info, 0};
        CHECK(hw_bios_call(16, registers) == HW_OK && unpacked == 0x04030200);
        info[7] = 0x80;
        CHECK(hw_bios_call(16, registers) == HW_OK && unpacked == 0x04030201);
        info[2] = 3;
        CHECK(hw_bios_call(16, registers) == HW_BAD_DATA && unpacked == 0x04030201);
    }
    return 0;
}

static uint32_t fuzz_next(uint32_t *state)
{
    *state = *state * 1664525u + 1013904223u;
    return *state;
}

static int decoder_fuzz_tests(void)
{
    static const unsigned operations[] = {0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18};
    static const uint8_t types[] = {0x10,0x10,0x28,0x30,0x30,0x81,0x81,0x82};
    uint32_t random = 0x74657374;
    unsigned trial, i, length, kind;
    uint8_t encoded[96], expected[80], guarded[96];
    size_t written;
    for (length = 0; length <= sizeof(expected); ++length) {
        for (i = 0; i < length; ++i) expected[i] = (uint8_t)fuzz_next(&random);
        for (kind = 0; kind < 5; ++kind) {
            size_t size = 4;
            unsigned operation = kind == 0 ? 0x11 : kind == 1 ? 0x14 : kind == 2 ? 0x16 : 0x13;
            encoded[0] = kind == 0 ? 0x10 : kind == 1 ? 0x30 : kind == 2 ? 0x81 : kind == 3 ? 0x28 : 0x24;
            encoded[1] = (uint8_t)length; encoded[2] = encoded[3] = 0;
            if (kind == 0) {
                for (i = 0; i < length; ++i) {
                    if (!(i & 7)) encoded[size++] = 0; /* all literal symbols */
                    encoded[size++] = expected[i];
                }
            } else if (kind == 1) {
                if (length) encoded[size++] = (uint8_t)(length - 1);
                for (i = 0; i < length; ++i) encoded[size++] = expected[i];
            } else if (kind == 2) {
                for (i = 0; i < length; ++i) encoded[size++] = (uint8_t)(expected[i] - (i ? expected[i - 1] : 0));
            } else {
                unsigned symbols = kind == 3 ? length : length * 2;
                unsigned symbol, bits_left = 0;
                uint32_t bits = 0;
                encoded[4] = 1; encoded[5] = 0xc0;
                encoded[6] = kind == 3 ? 0x32 : 3; encoded[7] = kind == 3 ? 0xa7 : 12;
                size = 8;
                for (i = 0; i < length; ++i) expected[i] = 0;
                for (symbol = 0; symbol < symbols; ++symbol) {
                    uint8_t value;
                    if (!bits_left) {
                        bits = fuzz_next(&random); bits_left = 32;
                        for (i = 0; i < 4; ++i) encoded[size++] = (uint8_t)(bits >> (i * 8));
                    }
                    value = encoded[6 + (bits >> 31)]; bits <<= 1; --bits_left;
                    if (kind == 3) expected[symbol] = value;
                    else expected[symbol / 2] |= (uint8_t)(value << ((symbol & 1) * 4));
                }
            }
            for (i = 0; i < sizeof(guarded); ++i) guarded[i] = 0xa5;
            CHECK(hw_bios_decompress(operation, encoded, size, guarded + 8, length, &written) == HW_OK && written == length);
            for (i = 0; i < length; ++i) CHECK(guarded[8 + i] == expected[i]);
            for (i = 0; i < 8; ++i) CHECK(guarded[i] == 0xa5);
            for (i = 8 + length; i < sizeof(guarded); ++i) CHECK(guarded[i] == 0xa5);
        }
    }
    for (trial = 0; trial < 20000; ++trial) {
        size_t input_size = fuzz_next(&random) % (sizeof(encoded) + 1);
        size_t capacity = fuzz_next(&random) % (sizeof(expected) + 1);
        uint8_t *input = encoded + sizeof(encoded) - input_size;
        int result;
        kind = (fuzz_next(&random) >> 16) % 8;
        for (i = 0; i < input_size; ++i) input[i] = (uint8_t)(fuzz_next(&random) >> 24);
        if (input_size >= 4 && (trial & 1)) {
            input[0] = types[kind]; input[1] = (uint8_t)(fuzz_next(&random) % 81);
            input[2] = input[3] = 0;
        }
        for (i = 0; i < sizeof(guarded); ++i) guarded[i] = 0xa5;
        result = hw_bios_decompress(operations[kind], input, input_size, guarded + 8, capacity, &written);
        CHECK(result == HW_OK || result == HW_TRUNCATED || result == HW_BAD_DATA || result == HW_NO_SPACE);
        CHECK(written <= capacity);
        if (result == HW_OK) {
            CHECK(input_size >= 4 && written == ((unsigned)input[1] | (unsigned)input[2] << 8 | (unsigned)input[3] << 16));
        }
        for (i = 0; i < 8; ++i) CHECK(guarded[i] == 0xa5);
        for (i = 8 + (unsigned)capacity; i < sizeof(guarded); ++i) CHECK(guarded[i] == 0xa5);
    }
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
    hw_write8(hw, HW_IO_BASE + 0x101, 0x80);
    CHECK(hw->timers[0].reload == 0x80fc && hw->timers[0].counter == 0xfffd);
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
    hw_scanline_start(hw, 3);
    hw_write16(hw, HW_IO_BASE + 4, 0x0438);
    hw_write16(hw, HW_IO_BASE + 0x202, 0xffff);
    hw_scanline_start(hw, 4);
    CHECK((hw_read16(hw, HW_IO_BASE + 4) & 7) == 4 && hw_read16(hw, HW_IO_BASE + 6) == 4);
    CHECK(hw_read16(hw, HW_IO_BASE + 0x202) == 4);
    hw_write16(hw, HW_IO_BASE + 0x202, 0xffff);
    hw_hblank(hw, 4);
    CHECK((hw_read16(hw, HW_IO_BASE + 4) & 7) == 6 && hw_read16(hw, HW_IO_BASE + 0x202) == 2);
    hw_scanline_start(hw, 5);
    CHECK(!(hw_read16(hw, HW_IO_BASE + 4) & 7));
    hw_write16(hw, HW_IO_BASE + 4, 0x043f); /* software cannot set status bits */
    CHECK(!(hw_read16(hw, HW_IO_BASE + 4) & 7));
    hw_vblank(hw);
    CHECK((hw_read16(hw, HW_IO_BASE + 4) & 7) == 1);
    hw_scanline_start(hw, 227);
    CHECK(!(hw_read16(hw, HW_IO_BASE + 4) & 3));
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

static int rtc_sdk_tests(void)
{
    HwRtcRecord record = {0x24, 0x12, 0x31, 2, 0x23, 0x59, 0x58, 0, 0x15, 0x42};
    HwRtcRecord readback = {0};
    Rtc_Initialize();
    CHECK(machine.gpio_enable && !iwram[0x35c6]);
    CHECK(Rtc_CheckAndReset() == 1);
    CHECK(Rtc_WriteDateTime(&record) == 1);
    CHECK(Rtc_ReadDateTime(&readback) == 1);
    CHECK(readback.year == 0x24 && readback.month == 0x12 && readback.day == 0x31);
    CHECK(readback.hour == 0x23 && readback.minute == 0x59 && readback.second == 0x58);
    clock_seconds += 2;
    CHECK(Rtc_ReadDateTime(&readback) == 1);
    CHECK(readback.year == 0x25 && readback.month == 1 && readback.day == 1 && readback.hour == 0 && readback.second == 0);
    record.hour = 0x12; record.minute = 0x34; record.second = 0x56;
    CHECK(sub_08248E70(&record) == 1 && Rtc_ReadTime(&readback) == 1);
    CHECK(readback.hour == 0x12 && readback.minute == 0x34 && readback.second == 0x56 && readback.year == 0x25);
    record.status = 7;
    CHECK(Rtc_WriteStatus(&record) == 1 && Rtc_ReadStatus(&readback) == 1);
    CHECK(readback.status == 0x47 && machine.rtc_status == 0x6a);
    CHECK(sub_08248F0C(&record) == 1 && machine.rtc_alarm[0] == 0x95 && machine.rtc_alarm[1] == 0x42);
    Rtc_Protect();
    CHECK(!machine.gpio_enable && iwram[0x35c6] == 1);
    CHECK(Rtc_ReadDateTime(&readback) == 0 && Rtc_WriteDateTime(&record) == 0 && Rtc_Reset() == 0);
    Rtc_Initialize();
    CHECK(Rtc_Reset() == 1 && Rtc_ReadStatus(&readback) == 1 && readback.status == 0x40);
    CHECK(Rtc_ReadDateTime(NULL) == 0 && Rtc_WriteStatus(NULL) == 0);
    hw_write16(&machine, 0x080000c4, 1); hw_write16(&machine, 0x080000c4, 5);
    hw_write16(&machine, 0x080000c6, 7); Rtc_WriteCommandByte(0x60);
    hw_write16(&machine, 0x080000c4, 1);
    CHECK(Rtc_ReadStatus(&readback) == 1 && readback.status == 0);
    CHECK(Rtc_CheckAndReset() == 0x11 && Rtc_ReadStatus(&readback) == 1 && readback.status == 0x40);
    return 0;
}

static int audio_tests(void)
{
    static struct SoundInfo sound;
    static const struct {
        uint16_t type, status;
        uint32_t frequency, loop, size;
        int8_t data[4];
    } wave = {0, 0x4000, 4000 * 1024, 0, 4, {0, 64, 0, -64}};
    struct SoundChannel *c = sound.chans;
    int16_t pcm[1024 * 2];
    unsigned i;
    int positive = 0, negative = 0;
    sound.maxChans = 1; sound.masterVolume = 15;
    c->wav = (struct WaveData *)&wave;
    c->statusFlags = SOUND_CHANNEL_SF_START;
    c->attack = 255; c->decay = 255; c->sustain = 128; c->release = 128;
    c->leftVolume = c->rightVolume = 255; c->frequency = 4000;
    CHECK(hw_mp2k_mix(&sound, pcm, 4, 8000) == 0);
    CHECK(pcm[0] == 0 && pcm[2] == 8128 && pcm[4] == 16256 && pcm[6] == 8128);
    CHECK(c->count == 2 && c->fw == 0 && c->envelopeVolume == 255);
    CHECK(hw_mp2k_mix(&sound, pcm, 12, 8000) == 0 && c->count == 4);
    c->statusFlags |= SOUND_CHANNEL_SF_STOP;
    CHECK(hw_mp2k_mix(&sound, pcm, 4, 8000) == 0 && c->envelopeVolume == 127);
    for (i = 0; i < 8; ++i) CHECK(hw_mp2k_mix(&sound, pcm, 4, 8000) == 0);
    CHECK(!c->statusFlags);
    for (i = 0; i < PCM_DMA_BUF_SIZE * 2; ++i) sound.pcmBuffer[i] = 0;
    sound.pcmDmaCounter = 2; sound.pcmDmaPeriod = 3; sound.reverb = 64;
    sound.pcmBuffer[0] = sound.pcmBuffer[PCM_DMA_BUF_SIZE] = -2;
    sound.pcmBuffer[8] = sound.pcmBuffer[PCM_DMA_BUF_SIZE + 8] = -2;
    sound.pcmBuffer[1] = sound.pcmBuffer[PCM_DMA_BUF_SIZE + 1] = 50;
    sound.pcmBuffer[9] = sound.pcmBuffer[PCM_DMA_BUF_SIZE + 9] = -2;
    CHECK(hw_mp2k_mix(&sound, pcm, 4, 8000) == HW_OK);
    CHECK(pcm[0] == 0 && pcm[1] == 0 && pcm[2] == 3072 && pcm[3] == 3072);
    CHECK(sound.pcmBuffer[9] == 12 && sound.pcmBuffer[1] == 50);
    hw_write16(&machine, HW_IO_BASE + 0x84, 0x80);
    hw_write16(&machine, HW_IO_BASE + 0x80, 0x1177);
    hw_write16(&machine, HW_IO_BASE + 0x82, 2);
    hw_write16(&machine, HW_IO_BASE + 0x62, 0xf080);
    hw_write16(&machine, HW_IO_BASE + 0x64, 0x8600);
    CHECK(machine.psg[0].enabled && (hw_read16(&machine, HW_IO_BASE + 0x84) & 1));
    hw_psg_render(&machine, pcm, 1024, 32768, 0);
    for (i = 0; i < 1024; ++i) { positive |= pcm[2 * i] > 0; negative |= pcm[2 * i] < 0; }
    CHECK(positive && negative);
    hw_write16(&machine, HW_IO_BASE + 0x62, 0xf0bf); /* one length tick */
    hw_write16(&machine, HW_IO_BASE + 0x64, 0xc600);
    hw_psg_render(&machine, pcm, 1024, 32768, 0);
    CHECK(!machine.psg[0].enabled);
    hw_write16(&machine, HW_IO_BASE + 0x78, 0xf000);
    hw_write16(&machine, HW_IO_BASE + 0x7c, 0x8009);
    CHECK(machine.psg[3].enabled);
    hw_psg_render(&machine, pcm, 32, 32768, 0);
    CHECK(machine.psg[3].lfsr != 0x7fff);
    hw_write16(&machine, HW_IO_BASE + 0x62, 0xf03d); /* three length ticks */
    hw_write16(&machine, HW_IO_BASE + 0x64, 0xc600);
    hw_psg_render(&machine, pcm, 128, 32768, 0);
    {
        uint32_t length = machine.psg[0].length;
        CHECK(length < 3);
        hw_write8(&machine, HW_IO_BASE + 0x63, 0xe0);
        CHECK(machine.psg[0].length == length); /* envelope byte is not length */
        hw_write8(&machine, HW_IO_BASE + 0x63, 0);
        CHECK(!machine.psg[0].enabled);
    }
    hw_write16(&machine, HW_IO_BASE + 0x84, 0);
    CHECK(!machine.psg[3].enabled);
    hw_write16(&machine, HW_IO_BASE + 0x84, 0x8f);
    CHECK(hw_read16(&machine, HW_IO_BASE + 0x84) == 0x80);
    hw_write16(&machine, HW_IO_BASE + 0x70, 0x40); /* CPU writes bank 0 */
    for (i = 0; i < 16; ++i) hw_write8(&machine, HW_IO_BASE + 0x90 + i, 0x11);
    hw_write16(&machine, HW_IO_BASE + 0x70, 0); /* CPU writes bank 1 */
    CHECK(hw_read8(&machine, HW_IO_BASE + 0x90) == 0);
    for (i = 0; i < 16; ++i) hw_write8(&machine, HW_IO_BASE + 0x90 + i, 0xee);
    hw_write16(&machine, HW_IO_BASE + 0x70, 0x40);
    CHECK(hw_read8(&machine, HW_IO_BASE + 0x90) == 0x11);
    hw_write16(&machine, HW_IO_BASE + 0x80, 0x4477);
    hw_write16(&machine, HW_IO_BASE + 0x82, 2);
    hw_write16(&machine, HW_IO_BASE + 0x70, 0xa0); /* play both banks from 0 */
    hw_write16(&machine, HW_IO_BASE + 0x72, 0x2000);
    hw_write16(&machine, HW_IO_BASE + 0x74, 0x87ff);
    hw_psg_render(&machine, pcm, 64, HW_CPU_HZ / 8, 0);
    CHECK(pcm[0] < 0 && pcm[60] < 0 && pcm[62] > 0 && pcm[124] > 0 && pcm[126] < 0);
    CHECK(hw_read8(&machine, HW_IO_BASE + 0x90) == 0xee);
    return 0;
}

static struct SoundInfo bridge_sound;
static struct MusicPlayerInfo bridge_player;
static unsigned bridge_order, bridge_error, bridge_samples;

static void bridge_sequence(struct MusicPlayerInfo *player)
{
    if (bridge_order != 0 || player != &bridge_player || bridge_sound.ident != ID_NUMBER + 1) bridge_error = 1;
    bridge_order = 1;
    SoundMain(); /* The SDK lock must reject reentry before another callback. */
    if (bridge_order != 1) bridge_error = 1;
}

static void bridge_cgb(void)
{
    if (bridge_order != 1 || bridge_sound.ident != ID_NUMBER + 1) bridge_error = 1;
    bridge_order = 2;
    /* Simulate the game's direct register stores after SoundInit powered on. */
    io[0x62] = 0x80; io[0x63] = 0xf0;
    io[0x64] = 0x00; io[0x65] = 0x86;
    io[0x80] = 0x77; io[0x81] = 0x11; io[0x82] = 2;
}

static void bridge_audio(void *user, const int16_t *stereo, size_t frames, uint32_t rate)
{
    size_t i;
    unsigned audible = 0;
    (void)user;
    if (bridge_order != 2 || bridge_sound.ident != ID_NUMBER || frames != 16 || rate != 8000) bridge_error = 1;
    for (i = 0; i < frames * 2; ++i) audible |= stereo[i] != 0;
    if (!audible) bridge_error = 1;
    bridge_samples += (unsigned)frames;
    bridge_order = 3;
}

static int sound_bridge_tests(void)
{
    HwHost host = machine.host;
    host.audio = bridge_audio;
    hw_init(&machine, &host);
    hw_m4a_attach(&machine, &bridge_sound);
    bridge_sound.ident = ID_NUMBER;
    bridge_sound.pcmSamplesPerVBlank = 16;
    bridge_sound.pcmFreq = 8000;
    bridge_sound.MPlayMainHead = bridge_sequence;
    bridge_sound.musicPlayerHead = &bridge_player;
    bridge_sound.CgbSound = bridge_cgb;
    bridge_order = bridge_error = bridge_samples = 0;
    io[0x84] = 0x80; /* SoundInit's direct store, pending import. */
    SoundMain();
    CHECK(!bridge_error && bridge_order == 3 && bridge_samples == 16 && machine.psg[0].enabled);
    CHECK(!(io[0x65] & 0x80) && bridge_sound.ident == ID_NUMBER);
    bridge_sound.ident = ID_NUMBER + 1;
    SoundMain();
    CHECK(bridge_samples == 16 && bridge_order == 3);
    bridge_sound.ident = ID_NUMBER;
    bridge_sound.pcmSamplesPerVBlank = PCM_DMA_BUF_SIZE + 1;
    SoundMain();
    CHECK(bridge_samples == 16);
    hw_m4a_attach(&machine, NULL);
    return 0;
}

static int sound_routing_tests(void)
{
    static struct SoundInfo sound;
    static const struct {
        uint16_t type, status;
        uint32_t frequency, loop, size;
        int8_t data[4];
    } wave = {0,0x4000,8000 * 1024,0,1,{64,0,0,0}};
    static const struct { uint16_t control; int16_t left, right; } cases[] = {
        {0x210c,8128,16256}, {0x120c,16256,8128}, {0x3300,12192,12192},
        {0x0100,0,8128}, {0x200c,8128,0}, {0,0,0}
    };
    HwHost host = machine.host;
    unsigned i, j;
    host.audio = NULL;
    hw_init(&machine, &host);
    hw_m4a_attach(&machine, &sound);
    sound.ident = ID_NUMBER; sound.pcmSamplesPerVBlank = 4; sound.pcmFreq = 8000;
    sound.masterVolume = 15; sound.maxChans = 1;
    sound.chans[0].wav = (struct WaveData *)&wave;
    sound.chans[0].type = TONEDATA_TYPE_FIX;
    sound.chans[0].attack = 255; sound.chans[0].leftVolume = 128; sound.chans[0].rightVolume = 255;
    hw_write16(&machine, HW_IO_BASE + 0x84, 0x80);
    for (i = 0; i < sizeof(cases) / sizeof(*cases); ++i) {
        hw_write16(&machine, HW_IO_BASE + 0x82, cases[i].control);
        sound.chans[0].statusFlags = SOUND_CHANNEL_SF_START;
        SoundMain();
        for (j = 0; j < 4; ++j) CHECK(machine.audio_buffer[j * 2] == cases[i].left && machine.audio_buffer[j * 2 + 1] == cases[i].right);
    }
    hw_write16(&machine, HW_IO_BASE + 0x82, 0x210c);
    hw_write16(&machine, HW_IO_BASE + 0x84, 0);
    sound.chans[0].statusFlags = SOUND_CHANNEL_SF_START;
    SoundMain();
    for (j = 0; j < 8; ++j) CHECK(machine.audio_buffer[j] == 0);
    CHECK(sound.chans[0].statusFlags != 0); /* Muting output does not stop sequencing. */
    hw_m4a_attach(&machine, NULL);
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
    result = decoder_fuzz_tests(); if (result) return result;
    result = device_tests(); if (result) return result;
    result = cartridge_tests(); if (result) return result;
    result = rtc_sdk_tests(); if (result) return result;
    result = audio_tests(); if (result) return result;
    result = sound_bridge_tests(); if (result) return result;
    return sound_routing_tests();
}
