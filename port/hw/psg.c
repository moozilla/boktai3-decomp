#include "internal.h"

static const unsigned length_register[4] = {0x62, 0x68, 0x72, 0x78};
static const unsigned envelope_register[4] = {0x63, 0x69, 0x73, 0x79};
static const unsigned frequency_register[4] = {0x64, 0x6c, 0x74, 0x7c};

static void publish_status(Hw *hw)
{
    unsigned i, status = hw->io[0x84] & 0x80;
    for (i = 0; i < 4; ++i) if (hw->psg[i].enabled) status |= 1u << i;
    hw->io[0x84] = (uint8_t)status;
    hw_publish_io(hw, 0x84, 2);
}

static void wave_window(Hw *hw)
{
    unsigned cpu_bank = ((hw->io[0x70] >> 6) & 1) ^ 1;
    hw_copy(hw->io + 0x90, hw->psg_wave[cpu_bank], 16);
    hw_publish_io(hw, 0x90, 16);
}

void hw_audio_io_write(Hw *hw, unsigned offset, uint16_t value, uint16_t mask)
{
    unsigned i;
    if (offset >= 0x90 && offset <= 0x9e) {
        unsigned cpu_bank = ((hw->io[0x70] >> 6) & 1) ^ 1;
        hw_put16(hw->psg_wave[cpu_bank] + (offset - 0x90), value);
        return;
    }
    if (offset == 0x70) wave_window(hw);
    if (offset == 0x84) {
        if (!(value & 0x80)) {
            hw_zero(hw->psg, sizeof(hw->psg));
            hw_zero(hw->io + 0x60, 0x22);
            hw_publish_io(hw, 0x60, 0x22);
            hw->psg_clock = hw->psg_fraction = hw->psg_step = 0;
        }
        publish_status(hw);
        return;
    }
    if (!(hw->io[0x84] & 0x80)) return;
    for (i = 0; i < 4; ++i) {
        HwPsgChannel *channel = hw->psg + i;
        uint8_t env = hw->io[envelope_register[i]];
        if (offset == length_register[i]) {
            if (mask & 0xff) channel->length = (i == 2 ? 256 : 64) - (hw->io[offset] & (i == 2 ? 255 : 63));
            if ((mask & 0xff00) && i != 2 && !(env & 0xf8)) channel->enabled = 0;
        }
        if (i == 2 && offset == 0x70 && !(value & 0x80)) channel->enabled = 0;
        if (offset != frequency_register[i] || !(mask & 0xff00) || !(value & 0x8000)) continue;
        channel->enabled = (uint8_t)(i == 2 ? (hw->io[0x70] & 0x80) != 0 : (env & 0xf8) != 0);
        channel->phase = 0;
        channel->lfsr = 0x7fff;
        if (!channel->length) channel->length = i == 2 ? 256 : 64;
        channel->volume = env >> 4;
        channel->envelope_period = env & 7;
        channel->envelope_up = (env >> 3) & 1;
        channel->envelope_clock = channel->envelope_period ? channel->envelope_period : 8;
        if (!i) {
            channel->sweep_shadow = value & 0x7ff;
            channel->sweep_clock = (hw->io[0x60] >> 4) & 7;
            if (!channel->sweep_clock) channel->sweep_clock = 8;
            if ((hw->io[0x60] & 7) && !(hw->io[0x60] & 8) && channel->sweep_shadow + (channel->sweep_shadow >> (hw->io[0x60] & 7)) > 2047) channel->enabled = 0;
        }
        hw->io[offset + 1] &= 0x7f;
        hw_publish_io(hw, offset, 2);
    }
    publish_status(hw);
}

static void frame_sequencer(Hw *hw)
{
    unsigned i, step = hw->psg_step;
    for (i = 0; i < 4; ++i) {
        HwPsgChannel *c = hw->psg + i;
        if (!(step & 1) && (hw->io[frequency_register[i] + 1] & 0x40) && c->length) {
            if (--c->length == 0) c->enabled = 0;
        }
        if (step == 7 && i != 2 && c->envelope_period && c->enabled) {
            if (c->envelope_clock && --c->envelope_clock == 0) {
                c->envelope_clock = c->envelope_period;
                if (c->envelope_up && c->volume < 15) ++c->volume;
                else if (!c->envelope_up && c->volume) --c->volume;
            }
        }
    }
    if (step == 2 || step == 6) {
        HwPsgChannel *c = hw->psg;
        uint8_t sweep = hw->io[0x60];
        if (c->sweep_clock && --c->sweep_clock == 0) {
            c->sweep_clock = (sweep >> 4) & 7;
            if (!c->sweep_clock) c->sweep_clock = 8;
            if ((sweep & 0x70) && (sweep & 7)) {
                unsigned delta = c->sweep_shadow >> (sweep & 7);
                unsigned next = sweep & 8 ? c->sweep_shadow - delta : c->sweep_shadow + delta;
                if (next > 2047) c->enabled = 0;
                else {
                    c->sweep_shadow = (uint16_t)next;
                    hw->io[0x64] = (uint8_t)next;
                    hw->io[0x65] = (uint8_t)((hw->io[0x65] & 0xf8) | (next >> 8));
                    hw_publish_io(hw, 0x64, 2);
                    if (!(sweep & 8) && next + (next >> (sweep & 7)) > 2047) c->enabled = 0;
                }
            }
        }
    }
    hw->psg_step = (uint8_t)((step + 1) & 7);
    publish_status(hw);
}

static int oscillator(Hw *hw, unsigned i, uint32_t cycles)
{
    static const uint8_t duty_pattern[4] = {0x01, 0x81, 0x87, 0x7e};
    static const unsigned noise_divisor[8] = {8,16,32,48,64,80,96,112};
    HwPsgChannel *c = hw->psg + i;
    unsigned frequency = hw_get16(hw->io + frequency_register[i]) & 0x7ff;
    uint64_t period;
    unsigned index;
    if (!c->enabled) return 0;
    c->phase += cycles;
    if (i == 3) {
        uint8_t nr43 = hw->io[0x7c];
        period = (uint64_t)noise_divisor[nr43 & 7] * 4 << (nr43 >> 4);
        while (c->phase >= period) {
            unsigned feedback = (c->lfsr ^ (c->lfsr >> 1)) & 1;
            c->phase -= period;
            c->lfsr = (uint16_t)((c->lfsr >> 1) | feedback << 14);
            if (nr43 & 8) c->lfsr = (uint16_t)((c->lfsr & ~0x40) | feedback << 6);
        }
        return c->lfsr & 1 ? -(int)c->volume : c->volume;
    }
    period = (i == 2 ? 8u : 16u) * (2048 - frequency);
    if (i == 2) {
        /* CPU sees the bank opposite the one currently playing. In 64-sample
         * mode the playback bank switches every 32 samples. */
        unsigned code = (hw->io[0x73] >> 5) & 3;
        int value;
        if ((hw->io[0x70] & 0x20) && ((c->phase / (period * 32)) & 1)) {
            hw->io[0x70] ^= 0x40;
            hw_publish_io(hw, 0x70, 2);
            wave_window(hw);
        }
        c->phase %= period * 32;
        index = (unsigned)(c->phase / period);
        value = hw->psg_wave[(hw->io[0x70] >> 6) & 1][index / 2];
        value = index & 1 ? value & 15 : value >> 4;
        value = value * 2 - 15;
        if (hw->io[0x73] & 0x80) return value * 3 / 4;
        return code ? value / (1 << (code - 1)) : 0;
    }
    index = (unsigned)(c->phase / period) & 7;
    c->phase %= period * 8;
    return (duty_pattern[hw->io[length_register[i]] >> 6] >> index) & 1 ? c->volume : -(int)c->volume;
}

void hw_psg_render(Hw *hw, int16_t *stereo, size_t frames, uint32_t rate, int add)
{
    size_t sample;
    if (!stereo || !rate) return;
    for (sample = 0; sample < frames; ++sample) {
        int32_t left = add ? stereo[2 * sample] : 0;
        int32_t right = add ? stereo[2 * sample + 1] : 0;
        unsigned i;
        uint64_t total = (uint64_t)hw->psg_fraction + HW_CPU_HZ;
        uint32_t cycles = (uint32_t)(total / rate);
        hw->psg_fraction = (uint32_t)(total % rate);
        if (hw->io[0x84] & 0x80) {
            hw->psg_clock += cycles;
            while (hw->psg_clock >= 32768) { hw->psg_clock -= 32768; frame_sequencer(hw); }
            for (i = 0; i < 4; ++i) {
                unsigned ratio = hw->io[0x82] & 3;
                int value = oscillator(hw, i, cycles) * (ratio >= 2 ? 64 : 16 << ratio);
                uint8_t route = hw->io[0x81];
                if (route & (0x10 << i)) left += value * (((hw->io[0x80] >> 4) & 7) + 1);
                if (route & (1 << i)) right += value * ((hw->io[0x80] & 7) + 1);
            }
        }
        stereo[2 * sample] = (int16_t)(left > 32767 ? 32767 : left < -32768 ? -32768 : left);
        stereo[2 * sample + 1] = (int16_t)(right > 32767 ? 32767 : right < -32768 ? -32768 : right);
    }
}
