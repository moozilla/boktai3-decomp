#include "internal.h"

static Hw *bound_hw;

uint8_t *hw_map(Hw *hw, uint32_t address, size_t size)
{
    return hw->host.map ? hw->host.map(hw->host.user, address, size) : NULL;
}

void hw_publish_io(Hw *hw, unsigned offset, unsigned size)
{
    uint8_t *p = hw_map(hw, HW_IO_BASE + offset, size);
    if (p) hw_copy(p, hw->io + offset, size);
}

void hw_bind(Hw *hw) { bound_hw = hw; }
Hw *hw_bound(void) { return bound_hw; }

void hw_init(Hw *hw, const HwHost *host)
{
    hw_zero(hw, sizeof(*hw));
    if (host) hw->host = *host;
    hw->solar_threshold = 232;
    hw->rtc_status = 0x40;
    hw_save_configure(hw, HW_SAVE_EEPROM_8K);
    hw_put16(hw->io + 0x130, 0x3ff);
    hw_put16(hw->io + 0x088, 0x200);
    hw_publish_io(hw, 0, sizeof(hw->io));
}

void hw_request_interrupt(Hw *hw, uint16_t mask)
{
    uint16_t flags = hw_get16(hw->io + 0x202) | mask;
    uint16_t pending;
    hw_put16(hw->io + 0x202, flags);
    hw_publish_io(hw, 0x202, 2);
    pending = flags & hw_get16(hw->io + 0x200);
    if (pending && (hw_get16(hw->io + 0x208) & 1) && hw->host.interrupt)
        hw->host.interrupt(hw->host.user, pending);
}

static int is_eeprom(uint32_t a) { return (a >> 24) == 0x0d; }

static uint32_t mirrored(uint32_t a)
{
    switch (a >> 24) {
    case 2: return 0x02000000 | (a & 0x3ffff);
    case 3: return 0x03000000 | (a & 0x7fff);
    case 5: return 0x05000000 | (a & 0x3ff);
    case 6: {
        unsigned offset = a & 0x1ffff;
        if (offset >= 0x18000) offset -= 0x8000;
        return 0x06000000 | offset;
    }
    case 7: return 0x07000000 | (a & 0x3ff);
    case 8: case 9: case 10: case 11: case 12: return 0x08000000 | (a & 0x1ffffff);
    default: return a;
    }
}

uint8_t hw_read8(Hw *hw, uint32_t a)
{
    uint8_t *p;
    if (a >= HW_IO_BASE && a < HW_IO_BASE + sizeof(hw->io)) return hw->io[a - HW_IO_BASE];
    if ((a >> 24) == 0x0e || (a >> 24) == 0x0f) return hw_flash_read(hw, (uint16_t)a);
    if (is_eeprom(a)) return (uint8_t)hw_eeprom_read(hw);
    if (a >= 0x080000c4 && a <= 0x080000c9 && hw->gpio_enable)
        return (uint8_t)(hw_cartridge_read(hw, a & ~1u) >> ((a & 1) * 8));
    p = hw_map(hw, mirrored(a), 1);
    return p ? *p : 0;
}

uint16_t hw_read16(Hw *hw, uint32_t a)
{
    a &= ~1u;
    if (is_eeprom(a)) return hw_eeprom_read(hw);
    return (uint16_t)(hw_read8(hw, a) | (uint16_t)hw_read8(hw, a + 1) << 8);
}

uint32_t hw_read32(Hw *hw, uint32_t a)
{
    a &= ~3u;
    return (uint32_t)hw_read16(hw, a) | (uint32_t)hw_read16(hw, a + 2) << 16;
}

static void fifo_write(Hw *hw, unsigned fifo, uint8_t value)
{
    if (hw->fifo_count[fifo] < 32) {
        unsigned tail = (hw->fifo_read[fifo] + hw->fifo_count[fifo]) & 31;
        hw->fifo[fifo][tail] = (int8_t)value;
        ++hw->fifo_count[fifo];
    }
}

static void dma_transfer(Hw *hw, unsigned channel, int fifo)
{
    HwDma *dma = hw->dma + channel;
    unsigned width = fifo || (dma->control & 0x400) ? 4 : 2;
    unsigned source_mode = (dma->control >> 7) & 3;
    unsigned destination_mode = (dma->control >> 5) & 3;
    uint32_t count = fifo ? 4 : dma->remaining;
    uint32_t source = dma->source & ~(width - 1), dest = dma->destination & ~(width - 1);
    while (count--) {
        if (width == 4) hw_write32(hw, dest, hw_read32(hw, source));
        else hw_write16(hw, dest, hw_read16(hw, source));
        if (source_mode == 0) source += width;
        else if (source_mode == 1) source -= width;
        if (!fifo) {
            if (destination_mode == 0 || destination_mode == 3) dest += width;
            else if (destination_mode == 1) dest -= width;
        }
    }
    dma->source = source;
    dma->destination = destination_mode == 3 ? dma->initial_destination : dest;
    if (dma->control & 0x4000) hw_request_interrupt(hw, (uint16_t)(0x100 << channel));
    if (!(dma->control & 0x200) || !(dma->control & 0x3000)) {
        unsigned offset = 0xba + channel * 12;
        dma->enabled = 0;
        dma->control &= ~0x8000;
        hw_put16(hw->io + offset, dma->control);
        hw_publish_io(hw, offset, 2);
    }
}

static void dma_control(Hw *hw, unsigned channel, uint16_t value)
{
    HwDma *dma = hw->dma + channel;
    int was_enabled = dma->enabled;
    unsigned offset = 0xb0 + channel * 12;
    dma->control = value;
    dma->enabled = (value >> 15) & 1;
    if (dma->enabled && !was_enabled) {
        dma->source = hw_get32(hw->io + offset) & (channel == 0 ? 0x07ffffffu : 0x0fffffffu);
        dma->destination = hw_get32(hw->io + offset + 4) & (channel == 3 ? 0x0fffffffu : 0x07ffffffu);
        dma->initial_destination = dma->destination;
        dma->remaining = hw_get16(hw->io + offset + 8) & (channel == 3 ? 0xffff : 0x3fff);
        if (!dma->remaining) dma->remaining = channel == 3 ? 0x10000 : 0x4000;
        if (!(value & 0x3000)) dma_transfer(hw, channel, 0);
    }
}

static void io_write16(Hw *hw, unsigned offset, uint16_t value, uint16_t mask)
{
    uint16_t old = hw_get16(hw->io + offset);
    if (offset == 0x006 || offset == 0x130) return;
    if (offset == 0x202) value = old & ~value;
    if (offset == 0x004) value = (value & ~7u) | (old & 7);
    if (offset == 0x084) value = (value & 0x80) | (old & 15);
    if (offset >= 0x100 && offset < 0x110 && !(offset & 2)) {
        HwTimer *timer = hw->timers + ((offset - 0x100) / 4);
        timer->reload = value;
        if (!(timer->control & 0x80)) timer->counter = value;
        value = timer->counter;
    }
    hw_put16(hw->io + offset, value);
    hw_publish_io(hw, offset, 2);
    if (offset >= 0xba && offset <= 0xde && (offset - 0xba) % 12 == 0)
        dma_control(hw, (offset - 0xba) / 12, value);
    if (offset >= 0x102 && offset <= 0x10e && (offset & 3) == 2) {
        HwTimer *timer = hw->timers + ((offset - 0x102) / 4);
        if (!(timer->control & 0x80) && (value & 0x80)) {
            timer->counter = timer->reload;
            timer->fraction = 0;
            hw_put16(hw->io + offset - 2, timer->counter);
            hw_publish_io(hw, offset - 2, 2);
        }
        timer->control = value & 0xc7;
    }
    if (offset == 0x082) {
        if (value & 0x0800) hw->fifo_count[0] = hw->fifo_read[0] = 0;
        if (value & 0x8000) hw->fifo_count[1] = hw->fifo_read[1] = 0;
        hw_put16(hw->io + offset, value & ~0x8800);
        hw_publish_io(hw, offset, 2);
    }
    if (offset >= 0xa0 && offset <= 0xa6) {
        unsigned fifo = (offset - 0xa0) / 4;
        fifo_write(hw, fifo, (uint8_t)value);
        fifo_write(hw, fifo, (uint8_t)(value >> 8));
    }
    if (offset == 0x132) hw_set_keys(hw, hw->keys);
    if ((offset == 0x200 || offset == 0x208) && value != old) hw_request_interrupt(hw, 0);
    if (offset >= 0x60 && offset <= 0x9e) hw_audio_io_write(hw, offset, value, mask);
}

void hw_write8(Hw *hw, uint32_t a, uint8_t value)
{
    uint8_t *p;
    if (a >= HW_IO_BASE && a < HW_IO_BASE + sizeof(hw->io)) {
        unsigned offset = a - HW_IO_BASE;
        uint16_t word;
        if (offset >= 0xa0 && offset <= 0xa7) {
            fifo_write(hw, (offset - 0xa0) / 4, value);
            return;
        }
        if ((offset & ~1u) == 0x202) {
            io_write16(hw, 0x202, (uint16_t)value << ((offset & 1) * 8), 0xffff);
            return;
        }
        word = hw_get16(hw->io + (offset & ~1u));
        if (offset >= 0x100 && offset < 0x110 && !(offset & 2))
            word = hw->timers[(offset - 0x100) / 4].reload;
        word = (uint16_t)((word & (offset & 1 ? 0xff : 0xff00)) | (uint16_t)value << ((offset & 1) * 8));
        io_write16(hw, offset & ~1u, word, offset & 1 ? 0xff00 : 0x00ff);
        return;
    }
    if ((a >> 24) == 0x0e || (a >> 24) == 0x0f) { hw_flash_write(hw, (uint16_t)a, value); return; }
    if (a >= 0x080000c4 && a <= 0x080000c9) {
        uint16_t word = hw_cartridge_read(hw, a & ~1u);
        word = (uint16_t)((word & (a & 1 ? 0xff : 0xff00)) | (uint16_t)value << ((a & 1) * 8));
        hw_cartridge_write(hw, a & ~1u, word);
        return;
    }
    if ((a >> 24) >= 8) return;
    /* Byte stores to palette/VRAM duplicate the byte; OAM ignores them. */
    if ((a >> 24) == 5 || (a >> 24) == 6) {
        if ((a >> 24) == 6 && (a & 0x1ffff) >= ((hw_get16(hw->io) & 7) >= 3 ? 0x14000u : 0x10000u)) return;
        p = hw_map(hw, mirrored(a & ~1u), 2);
        if (p) p[0] = p[1] = value;
        return;
    }
    if ((a >> 24) == 7) return;
    p = hw_map(hw, mirrored(a), 1);
    if (p) *p = value;
}

void hw_write16(Hw *hw, uint32_t a, uint16_t value)
{
    uint8_t *p;
    a &= ~1u;
    if (a >= HW_IO_BASE && a < HW_IO_BASE + sizeof(hw->io)) { io_write16(hw, a - HW_IO_BASE, value, 0xffff); return; }
    if (a >= 0x080000c4 && a <= 0x080000c8) { hw_cartridge_write(hw, a, value); return; }
    if (is_eeprom(a)) { hw_eeprom_write(hw, value); return; }
    if ((a >> 24) == 0x0e || (a >> 24) == 0x0f) { hw_flash_write(hw, (uint16_t)a, (uint8_t)value); return; }
    if ((a >> 24) >= 8) return;
    p = hw_map(hw, mirrored(a), 2);
    if (p) hw_put16(p, value);
}

void hw_write32(Hw *hw, uint32_t a, uint32_t value)
{
    a &= ~3u;
    hw_write16(hw, a, (uint16_t)value);
    hw_write16(hw, a + 2, (uint16_t)(value >> 16));
}

void hw_sync_io(Hw *hw)
{
    /* Snapshot first: a DMA control write can change the mapped registers. */
    uint8_t snapshot[0x400];
    uint8_t *mapped = hw_map(hw, HW_IO_BASE, sizeof(snapshot));
    unsigned offset;
    if (!mapped) return;
    hw_copy(snapshot, mapped, sizeof(snapshot));
    for (offset = 0; offset < sizeof(snapshot); offset += 2) {
        uint16_t value = hw_get16(snapshot + offset);
        if (value != hw_get16(hw->io + offset)) io_write16(hw, offset, value, 0xffff);
    }
}

void hw_set_keys(Hw *hw, uint16_t pressed)
{
    uint16_t control = hw_get16(hw->io + 0x132);
    uint16_t mask = control & 0x3ff;
    int match;
    hw->keys = pressed & 0x3ff;
    hw_put16(hw->io + 0x130, (uint16_t)(~hw->keys & 0x3ff));
    hw_publish_io(hw, 0x130, 2);
    match = mask && ((control & 0x8000) ? (hw->keys & mask) == mask : (hw->keys & mask) != 0);
    if (match && !hw->key_irq_match && (control & 0x4000)) hw_request_interrupt(hw, 0x1000);
    hw->key_irq_match = (uint8_t)match;
}

static void fifo_timer(Hw *hw, unsigned timer, uint32_t overflows)
{
    unsigned f;
    uint16_t control = hw_get16(hw->io + 0x82);
    if (timer > 1) return;
    for (f = 0; f < 2; ++f) {
        uint32_t n;
        if (((control >> (10 + 4 * f)) & 1) != timer) continue;
        for (n = 0; n < overflows; ++n) {
            unsigned channel;
            if (hw->fifo_count[f]) {
                hw->fifo_sample[f] = hw->fifo[f][hw->fifo_read[f]];
                hw->fifo_read[f] = (hw->fifo_read[f] + 1) & 31;
                --hw->fifo_count[f];
            }
            if (hw->fifo_count[f] > 16) continue;
            for (channel = 1; channel <= 2; ++channel) {
                HwDma *dma = hw->dma + channel;
                if (dma->enabled && (dma->control & 0x3000) == 0x3000 && dma->destination == 0x040000a0 + 4 * f)
                    dma_transfer(hw, channel, 1);
            }
        }
    }
}

void hw_step_cycles(Hw *hw, uint32_t cycles)
{
    static const unsigned prescale[4] = {1, 64, 256, 1024};
    uint32_t overflow = 0;
    unsigned i;
    hw->cycles += cycles;
    for (i = 0; i < 4; ++i) {
        HwTimer *t = hw->timers + i;
        uint64_t ticks, total;
        uint32_t current_overflow = 0;
        if (!(t->control & 0x80)) { overflow = 0; continue; }
        if (i && (t->control & 4)) ticks = overflow;
        else {
            unsigned divisor = prescale[t->control & 3];
            ticks = (uint64_t)t->fraction + cycles;
            t->fraction = (uint32_t)(ticks % divisor);
            ticks /= divisor;
        }
        total = t->counter + ticks;
        if (total >= 0x10000) {
            uint32_t period = 0x10000u - t->reload;
            total -= 0x10000;
            current_overflow = 1 + (uint32_t)(total / period);
            t->counter = (uint16_t)(t->reload + total % period);
            if (t->control & 0x40) hw_request_interrupt(hw, (uint16_t)(8 << i));
            fifo_timer(hw, i, current_overflow);
        } else t->counter = (uint16_t)total;
        hw_put16(hw->io + 0x100 + i * 4, t->counter);
        hw_publish_io(hw, 0x100 + i * 4, 2);
        overflow = current_overflow;
    }
}

static void display_status(Hw *hw, unsigned scanline, int hblank)
{
    uint16_t old = hw_get16(hw->io + 4), status;
    hw->scanline = (uint16_t)(scanline % HW_LINES_PER_FRAME);
    status = (uint16_t)((old & ~7u) | (hblank ? 2 : 0) |
                       (hw->scanline >= 160 && hw->scanline < 227));
    if (hw->scanline == status >> 8) status |= 4;
    hw_put16(hw->io + 4, status);
    hw_put16(hw->io + 6, hw->scanline);
    hw_publish_io(hw, 4, 4);
    if ((status & 0x24) == 0x24 && !(old & 4)) hw_request_interrupt(hw, 4);
}

void hw_scanline_start(Hw *hw, unsigned scanline)
{
    display_status(hw, scanline, 0);
}

void hw_hblank(Hw *hw, unsigned scanline)
{
    unsigned channel;
    display_status(hw, scanline, 1);
    if (hw_get16(hw->io + 4) & 0x10) hw_request_interrupt(hw, 2);
    if (hw->scanline >= 160) return;
    for (channel = 0; channel < 4; ++channel)
        if (hw->dma[channel].enabled && (hw->dma[channel].control & 0x3000) == 0x2000)
            dma_transfer(hw, channel, 0);
}

void hw_vblank(Hw *hw)
{
    unsigned channel;
    display_status(hw, 160, 0);
    if (hw_get16(hw->io + 4) & 8) hw_request_interrupt(hw, 1);
    for (channel = 0; channel < 4; ++channel)
        if (hw->dma[channel].enabled && (hw->dma[channel].control & 0x3000) == 0x1000)
            dma_transfer(hw, channel, 0);
}
