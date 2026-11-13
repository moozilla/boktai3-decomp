#include "internal.h"
#include "gba/types.h"

static void report(unsigned swi, int result)
{
    Hw *hw = hw_bound();
    if (result && hw && hw->host.bios_error) hw->host.bios_error(hw->host.user, swi, result);
}

void RegisterRamReset(uint32_t flags)
{
    static const uint32_t addresses[] = {0x02000000, 0x03000000, 0x05000000, 0x06000000, 0x07000000};
    static const uint32_t sizes[] = {0x40000, 0x7e00, 0x400, 0x18000, 0x400};
    Hw *hw = hw_bound();
    unsigned i;
    if (!hw) { report(1, HW_BAD_DATA); return; }
    for (i = 0; i < 5; ++i) if (flags & (1u << i)) {
        uint8_t *p = hw_map(hw, addresses[i], sizes[i]);
        if (p) hw_zero(p, sizes[i]);
    }
    hw_write16(hw, HW_IO_BASE, 0x80);
    if (flags & 0x20) {
        for (i = 0x120; i <= 0x12a; i += 2) hw_write16(hw, HW_IO_BASE + i, 0);
        hw_write16(hw, HW_IO_BASE + 0x134, 0x8000);
        hw_write16(hw, HW_IO_BASE + 0x140, 0);
    }
    if (flags & 0x40) {
        for (i = 0x60; i < 0x88; i += 2) hw_write16(hw, HW_IO_BASE + i, 0);
        hw_write16(hw, HW_IO_BASE + 0x88, 0x200);
        hw_zero(hw->fifo_count, sizeof(hw->fifo_count));
    }
    if (flags & 0x80) {
        for (i = 8; i < 0x56; i += 2) hw_write16(hw, HW_IO_BASE + i, 0);
        hw_write16(hw, HW_IO_BASE + 0x20, 0x100);
        hw_write16(hw, HW_IO_BASE + 0x26, 0x100);
        hw_write16(hw, HW_IO_BASE + 0x30, 0x100);
        hw_write16(hw, HW_IO_BASE + 0x36, 0x100);
        for (i = 0xb0; i < 0x110; i += 2) hw_write16(hw, HW_IO_BASE + i, 0);
        hw_write16(hw, HW_IO_BASE + 0x200, 0);
        hw_write16(hw, HW_IO_BASE + 0x202, 0xffff);
    }
}

void SoftReset(uint32_t flags)
{
    Hw *hw = hw_bound();
    RegisterRamReset(flags);
    if (!hw) return;
    hw_write16(hw, HW_IO_BASE + 0x208, 0);
    if (hw->host.reset) hw->host.reset(hw->host.user);
    else report(0, HW_UNSUPPORTED);
}

void VBlankIntrWait(void)
{
    Hw *hw = hw_bound();
    if (hw && hw->host.wait) hw->host.wait(hw->host.user, 1, 1);
    else report(5, HW_UNSUPPORTED);
}

void SoundBiasReset(void)
{
    Hw *hw = hw_bound();
    if (hw) hw_write16(hw, HW_IO_BASE + 0x88, hw_read16(hw, HW_IO_BASE + 0x88) & 0xc000);
}

void SoundBiasSet(void)
{
    Hw *hw = hw_bound();
    if (hw) hw_write16(hw, HW_IO_BASE + 0x88, (hw_read16(hw, HW_IO_BASE + 0x88) & 0xc000) | 0x200);
}

static int bit_unpack(const uint8_t *src, uint8_t *dst, const uint8_t *info)
{
    unsigned length = hw_get16(info), source_width = info[2], dest_width = info[3];
    uint32_t offset = hw_get32(info + 4), word = 0;
    unsigned used = 0, i;
    if ((source_width != 1 && source_width != 2 && source_width != 4 && source_width != 8) ||
        (dest_width != 1 && dest_width != 2 && dest_width != 4 && dest_width != 8 && dest_width != 16 && dest_width != 32)) return HW_BAD_DATA;
    for (i = 0; i < length; ++i) {
        unsigned shift;
        for (shift = 0; shift < 8; shift += source_width) {
            uint32_t value = (src[i] >> shift) & ((1u << source_width) - 1);
            if (value || (offset & 0x80000000)) value += offset & 0x7fffffff;
            word |= value << used;
            used += dest_width;
            if (used == 32) { hw_put32(dst, word); dst += 4; used = 0; word = 0; }
        }
    }
    /* BIOS only flushes complete words. Inputs must expand to a word multiple. */
    return HW_OK;
}

int hw_bios_call(unsigned swi, uintptr_t r[4])
{
    Hw *hw = hw_bound();
    int32_t remainder, quotient;
    if (!r) return HW_BAD_DATA;
    switch (swi) {
    case 0:
        if (!hw || !hw->host.reset) return HW_UNSUPPORTED;
        hw->host.reset(hw->host.user);
        return HW_OK;
    case 1: RegisterRamReset((uint32_t)r[0]); return hw ? HW_OK : HW_BAD_DATA;
    case 2: case 3: case 4: case 5:
        if (!hw || !hw->host.wait) return HW_UNSUPPORTED;
        hw->host.wait(hw->host.user, swi == 5 ? 1 : swi == 4 ? (uint16_t)r[1] : swi == 3 ? 0x3080 : 0xffff,
                      swi == 5 || (swi == 4 && r[0]));
        return HW_OK;
    case 6: case 7:
        quotient = hw_bios_div((int32_t)r[swi == 7 ? 1 : 0], (int32_t)r[swi == 7 ? 0 : 1], &remainder);
        r[0] = (uint32_t)quotient; r[1] = (uint32_t)remainder;
        r[3] = quotient < 0 ? 0u - (uint32_t)quotient : (uint32_t)quotient;
        return HW_OK;
    case 8: r[0] = Sqrt((uint32_t)r[0]); return HW_OK;
    case 9: r[0] = (uint32_t)(int32_t)ArcTan((int16_t)r[0]); return HW_OK;
    case 10: r[0] = ArcTan2((int16_t)r[0], (int16_t)r[1]); return HW_OK;
    case 11: CpuSet((const void *)r[0], (void *)r[1], (uint32_t)r[2]); return HW_OK;
    case 12: CpuFastSet((const void *)r[0], (void *)r[1], (uint32_t)r[2]); return HW_OK;
    case 14: BgAffineSet((struct BgAffineSrcData *)r[0], (struct BgAffineDstData *)r[1], (int32_t)r[2]); return HW_OK;
    case 15: ObjAffineSet((struct ObjAffineSrcData *)r[0], (void *)r[1], (int32_t)r[2], (int32_t)r[3]); return HW_OK;
    case 16: return bit_unpack((const uint8_t *)r[0], (uint8_t *)r[1], (const uint8_t *)r[2]);
    case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24:
        return hw_bios_decompress(swi, (const void *)r[0], SIZE_MAX, (void *)r[1], SIZE_MAX, NULL);
    case 25:
        if (r[0]) SoundBiasSet(); else SoundBiasReset();
        return hw ? HW_OK : HW_BAD_DATA;
    default: return HW_UNSUPPORTED;
    }
}

void hw_system_call(unsigned swi)
{
    uintptr_t r[4] = {0, 0, 0, 0};
    report(swi, hw_bios_call(swi, r));
}
