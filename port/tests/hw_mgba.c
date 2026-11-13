/* Optional independent oracle. Reads the user's ROM; writes no ROM data. */
#include "hw.h"
#include "hw_rtc.h"
#include "gba/types.h"
#include <mgba/core/core.h>
#include <mgba/internal/arm/arm.h>
#include <mgba/internal/arm/isa-inlines.h>
#include <mgba/internal/gba/bios.h>
#include <mgba/internal/gba/gba.h>
#undef UNUSED
#include "gba/m4a_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static void quiet_log(struct mLogger *logger, int category, enum mLogLevel level, const char *format, va_list args)
{
    (void)logger; (void)category;
    if (level & (mLOG_FATAL | mLOG_ERROR)) vfprintf(stderr, format, args);
}
static struct mLogger logger = {quiet_log, NULL};

static void swi(struct mCore *core, unsigned n, uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
    struct ARMCore *cpu = core->cpu;
    cpu->gprs[0] = (int32_t)a; cpu->gprs[1] = (int32_t)b;
    cpu->gprs[2] = (int32_t)c; cpu->gprs[3] = (int32_t)d;
    GBASwi16(cpu, (int)n);
}

static int rom_call(struct mCore *core, uint32_t entry, uint32_t a, uint32_t b, uint32_t c)
{
    struct ARMCore *cpu = core->cpu;
    unsigned instructions;
    core->busWrite16(core, 0x0203f000, 0xe7fe);
    cpu->cpsr.i = 1; cpu->halted = 0;
    ARMSetPrivilegeMode(cpu, MODE_SYSTEM);
    _ARMSetMode(cpu, MODE_THUMB);
    cpu->gprs[0] = (int32_t)a; cpu->gprs[1] = (int32_t)b; cpu->gprs[2] = (int32_t)c;
    cpu->gprs[ARM_SP] = 0x03007d00; cpu->gprs[ARM_LR] = 0x0203f001;
    cpu->gprs[ARM_PC] = (int32_t)entry;
    ThumbWritePC(cpu);
    for (instructions = 0; instructions < 2000000; ++instructions) {
        if ((uint32_t)cpu->gprs[ARM_PC] == 0x0203f002) return 0;
        core->step(core);
    }
    fprintf(stderr, "ROM call %08X did not return (pc %08X)\n", entry, cpu->gprs[ARM_PC]);
    return 1;
}

static int eeprom_oracle(struct mCore *core)
{
    struct GBA *gba = core->board;
    static Hw hw;
    uint16_t data[] = {0x1234,0x5678,0x9abc,0xdef0};
    extern uint16_t Eeprom_WriteBlock(uint16_t, const uint16_t *, uint8_t);
    unsigned i;
    GBASavedataForceType(&gba->memory.savedata, SAVEDATA_EEPROM);
    if (rom_call(core, 0x082485ec, 64, 0, 0) || ((struct ARMCore *)core->cpu)->gprs[0]) return 1;
    for (i = 0; i < 4; ++i) core->busWrite16(core, 0x02003000 + i * 2, data[i]);
    if (rom_call(core, 0x08248778, 17, 0x02003000, 0) || ((struct ARMCore *)core->cpu)->gprs[0]) {
        fprintf(stderr,"Original EEPROM write failed\n"); return 1;
    }
    if (rom_call(core, 0x082486b4, 17, 0x02003020, 0) || ((struct ARMCore *)core->cpu)->gprs[0]) return 1;
    for (i = 0; i < 4; ++i) if (core->busRead16(core, 0x02003020 + i * 2) != data[i]) {
        fprintf(stderr, "Original EEPROM readback failed\n"); return 1;
    }
    hw_init(&hw, NULL); hw_bind(&hw);
    if (Eeprom_WriteBlock(17, data, 0)) return 1;
    for (i = 0; i < 8; ++i) if (hw.save[17 * 8 + i] != gba->memory.savedata.data[17 * 8 + i]) {
        fprintf(stderr,"EEPROM backing byte %u: original %u / port %u\n", i, gba->memory.savedata.data[17 * 8 + i], hw.save[17 * 8 + i]); return 1;
    }
    hw_bind(NULL);
    puts("Original EEPROM SDK: PASS (block readback and save-file bytes)");
    return 0;
}

static int mixer_oracle(struct mCore *core)
{
    struct ARMCore *cpu = core->cpu;
    struct SoundInfo sound = {0};
    struct { uint16_t type, status; uint32_t freq, loop, size; int8_t data[20]; } wave =
        {0, 0x4000, 4000 * 1024, 0, 16, {0,32,64,96,127,96,64,32,0,-32,-64,-96,-128,-96,-64,-32,0,0,0,0}};
    unsigned i, instructions;
    int16_t pcm[32];
    for (i = 0; i < 0x1c00; ++i) core->busWrite8(core, 0x02000000 + i, 0);
    for (i = 0; i < 0x400; ++i) core->busWrite8(core, 0x030035f0 + i, core->busRead8(core, 0x0822f248 + i));
    for (i = 0; i < sizeof(wave); ++i) core->busWrite8(core, 0x02002000 + i, ((uint8_t *)&wave)[i]);
    core->busWrite32(core, 0x03007ff0, 0x02000000);
    core->busWrite32(core, 0x02000000, ID_NUMBER);
    core->busWrite8(core, 0x02000006, 1);
    core->busWrite8(core, 0x02000007, 15);
    core->busWrite8(core, 0x0200000b, 1);
    core->busWrite32(core, 0x02000010, 16);
    core->busWrite32(core, 0x02000018, 1049);
    core->busWrite32(core, 0x02000028, 0x0822fe19); /* existing bx lr */
    core->busWrite8(core, 0x02000050, 0x80);
    core->busWrite8(core, 0x02000052, 255); core->busWrite8(core, 0x02000053, 255);
    core->busWrite8(core, 0x02000054, 255); core->busWrite8(core, 0x02000055, 255);
    core->busWrite8(core, 0x02000056, 128); core->busWrite8(core, 0x02000057, 128);
    core->busWrite32(core, 0x02000070, 4000); core->busWrite32(core, 0x02000074, 0x02002000);
    sound.maxChans = 1; sound.masterVolume = 15; sound.pcmDmaPeriod = 1;
    sound.pcmFreq = 8000; sound.divFreq = 1049;
    sound.chans[0].wav = (struct WaveData *)&wave;
    sound.chans[0].statusFlags = 0x80;
    sound.chans[0].leftVolume = sound.chans[0].rightVolume = 255;
    sound.chans[0].attack = sound.chans[0].decay = 255;
    sound.chans[0].sustain = sound.chans[0].release = 128;
    sound.chans[0].frequency = 4000;
    core->busWrite16(core, 0x0203f000, 0xe7fe);
    cpu->cpsr.i = 1; cpu->halted = 0;
    ARMSetPrivilegeMode(cpu, MODE_SYSTEM);
    _ARMSetMode(cpu, MODE_THUMB);
    cpu->gprs[ARM_SP] = 0x03007d00; cpu->gprs[ARM_LR] = 0x0203f001;
    cpu->gprs[ARM_PC] = 0x0822f1c4;
    ThumbWritePC(cpu);
    for (instructions = 0; instructions < 100000; ++instructions) {
        if ((uint32_t)cpu->gprs[ARM_PC] == 0x0203f002) break;
        core->step(core);
    }
    if (instructions == 100000) { fprintf(stderr, "Mixer call did not return (pc %08X)\n", cpu->gprs[ARM_PC]); return 1; }
    if (hw_mp2k_mix(&sound, pcm, 16, 8000)) return 1;
    FILE *comparison = fopen("build/demo/mixer-comparison.csv", "w");
    if (comparison) fputs("sample,original,portable\n", comparison);
    for (i = 0; i < 16; ++i) {
        int reference = (int8_t)core->busRead8(core, 0x02000350 + i);
        int actual = sound.pcmBuffer[i];
        if (comparison) fprintf(comparison, "%u,%d,%d\n", i, reference, actual);
        if (abs(reference - actual) > 1) {
            fprintf(stderr, "Mixer sample %u: original %d / port %d\n", i, reference, actual); return 1;
        }
    }
    if (comparison) fclose(comparison);
    if (core->busRead32(core, 0x0200006c) != sound.chans[0].fw || core->busRead32(core, 0x02000068) != sound.chans[0].count) {
        fprintf(stderr, "Mixer channel position differs\n"); return 1;
    }
    puts("Original ROM mixer: PASS (PCM within one 8-bit unit, exact phase/count)");
    return 0;
}

static int mixer_state_oracle(struct mCore *core)
{
    unsigned variant, frame, i, cases = 0;
    for (variant = 0; variant < 8; ++variant) {
        struct SoundInfo sound = {0};
        struct { uint16_t type, status; uint32_t freq, loop, size; int8_t data[36]; } wave =
            {0, 0x4000, 4000 * 1024, 0, 32, {0}};
        struct SoundChannel *c = sound.chans;
        int16_t pcm[32];
        for (i = 0; i < 32; ++i) wave.data[i] = (int8_t)((i < 16 ? i : 32 - i) * 7 - 56);
        wave.data[32] = wave.data[0]; /* interpolation guard agrees with loop */
        if (variant & 4) { wave.status = 0; wave.data[32] = 0; }
        for (i = 0; i < 0x1c00; ++i) core->busWrite8(core, 0x02000000 + i, 0);
        for (i = 0; i < sizeof(wave); ++i) core->busWrite8(core, 0x02002000 + i, ((uint8_t *)&wave)[i]);
        sound.maxChans = 1; sound.masterVolume = 11; sound.pcmDmaPeriod = 1;
        sound.pcmFreq = 8000; sound.divFreq = 1049;
        c->wav = (struct WaveData *)&wave; c->statusFlags = 0x80;
        c->type = variant & 1 ? TONEDATA_TYPE_FIX : 0;
        c->leftVolume = 230; c->rightVolume = 130;
        c->attack = variant & 2 ? 64 : 255; c->decay = 180;
        c->sustain = 70; c->release = 128; c->frequency = 4000;
        c->pseudoEchoVolume = 12; c->pseudoEchoLength = 3;
        core->busWrite32(core, 0x02000000, ID_NUMBER);
        core->busWrite8(core, 0x02000006, sound.maxChans);
        core->busWrite8(core, 0x02000007, sound.masterVolume);
        core->busWrite8(core, 0x0200000b, 1);
        core->busWrite32(core, 0x02000010, 16);
        core->busWrite32(core, 0x02000018, 1049);
        core->busWrite32(core, 0x02000028, 0x0822fe19);
        for (i = 0; i < 36; ++i) core->busWrite8(core, 0x02000050 + i, ((uint8_t *)c)[i]);
        core->busWrite32(core, 0x02000074, 0x02002000);
        for (frame = 0; frame < 12; ++frame) {
            if (frame == 5 && c->statusFlags) {
                c->statusFlags |= 0x40;
                core->busWrite8(core, 0x02000050, core->busRead8(core, 0x02000050) | 0x40);
            }
            if (rom_call(core, 0x0822f1c4, 0, 0, 0) || hw_mp2k_mix(&sound, pcm, 16, 8000)) return 1;
            for (i = 0; i < 16; ++i) {
                int ref_right = (int8_t)core->busRead8(core, 0x02000350 + i);
                int ref_left = (int8_t)core->busRead8(core, 0x02000980 + i);
                if (abs(ref_right - sound.pcmBuffer[i]) > 1 || abs(ref_left - sound.pcmBuffer[PCM_DMA_BUF_SIZE + i]) > 1) {
                    fprintf(stderr, "Mixer case %u frame %u sample %u: original %d/%d port %d/%d\n", variant, frame, i, ref_left, ref_right, sound.pcmBuffer[PCM_DMA_BUF_SIZE + i], sound.pcmBuffer[i]); return 1;
                }
            }
            if (core->busRead8(core, 0x02000050) != c->statusFlags ||
                (c->statusFlags && (core->busRead8(core, 0x02000059) != c->envelopeVolume ||
                 core->busRead32(core, 0x02000068) != c->count || core->busRead32(core, 0x0200006c) != c->fw))) {
                fprintf(stderr, "Mixer state case %u frame %u: flags %u/%u env %u/%u count %u/%u phase %u/%u\n", variant, frame,
                    core->busRead8(core, 0x02000050), c->statusFlags, core->busRead8(core, 0x02000059), c->envelopeVolume,
                    core->busRead32(core, 0x02000068), c->count, core->busRead32(core, 0x0200006c), c->fw); return 1;
            }
            ++cases;
        }
    }
    printf("Original mixer state: PASS (%u frames: fixed/interpolated, loop/end, ADSR and pseudo echo)\n", cases);
    return 0;
}

static int mixer_reverb_oracle(struct mCore *core)
{
    static const unsigned reverbs[] = {1, 32, 64, 127};
    unsigned counter, level, i, cases = 0;
    for (counter = 0; counter <= 3; ++counter) for (level = 0; level < 4; ++level) {
        struct SoundInfo sound = {0};
        int16_t pcm[32];
        for (i = 0; i < 0x350; ++i) core->busWrite8(core, 0x02000000 + i, 0);
        sound.reverb = (uint8_t)reverbs[level];
        sound.pcmDmaCounter = (uint8_t)counter;
        sound.pcmDmaPeriod = 3;
        for (i = 0; i < PCM_DMA_BUF_SIZE * 2; ++i) {
            sound.pcmBuffer[i] = (int8_t)((i * 71 % 201) - 100);
            core->busWrite8(core, 0x02000350 + i, (uint8_t)sound.pcmBuffer[i]);
        }
        core->busWrite32(core, 0x02000000, ID_NUMBER);
        core->busWrite8(core, 0x02000004, sound.pcmDmaCounter);
        core->busWrite8(core, 0x02000005, sound.reverb);
        core->busWrite8(core, 0x0200000b, sound.pcmDmaPeriod);
        core->busWrite32(core, 0x02000010, 16);
        core->busWrite32(core, 0x02000028, 0x0822fe19);
        if (rom_call(core, 0x0822f1c4, 0, 0, 0) || hw_mp2k_mix(&sound, pcm, 16, 8000)) return 1;
        for (i = 0; i < PCM_DMA_BUF_SIZE * 2; ++i) if ((int8_t)core->busRead8(core, 0x02000350 + i) != sound.pcmBuffer[i]) {
            fprintf(stderr, "Reverb counter %u level %u byte %u differs\n", counter, reverbs[level], i); return 1;
        }
        ++cases;
    }
    printf("Original mixer reverb: PASS (%u ring/feedback cases, exact full buffer)\n", cases);
    return 0;
}

static int bios_memory_oracle(struct mCore *core)
{
    struct ARMCore *cpu = core->cpu;
    uint32_t random = 0x13579bdf, source[80], actual[80];
    unsigned i, variant, count, cases = 0;
    for (i = 0; i < 2048; ++i) {
        int32_t numerator, denominator, remainder;
        random = random * 1664525u + 1013904223u;
        swi(core, 8, random, 0, 0, 0);
        if ((uint32_t)cpu->gprs[0] != Sqrt(random)) return 1;
        numerator = (int32_t)random;
        random = random * 1664525u + 1013904223u;
        denominator = (int32_t)(random | 1);
        swi(core, 6, (uint32_t)numerator, (uint32_t)denominator, 0, 0);
        if (cpu->gprs[0] != hw_bios_div(numerator, denominator, &remainder) || cpu->gprs[1] != remainder) return 1;
        cases += 2;
    }
    for (i = 0; i < 80; ++i) source[i] = 0x01234567u + i * 0x01010101u;
    for (variant = 0; variant < 6; ++variant) for (count = 1; count <= 65; ++count) {
        unsigned swi_number = variant >= 4 ? 12 : 11;
        uint32_t control = count | (variant & 1 ? 0x01000000 : 0) | (variant >= 2 ? 0x04000000 : 0);
        for (i = 0; i < 80; ++i) {
            actual[i] = 0xa5a5a5a5;
            core->busWrite32(core, 0x02000000 + i * 4, source[i]);
            core->busWrite32(core, 0x02001000 + i * 4, actual[i]);
        }
        if (swi_number == 12) CpuFastSet(source, actual, control);
        else CpuSet(source, actual, control);
        /* mGBA executes copy SWIs through its replacement BIOS, so run the
         * Thumb caller through return instead of assuming immediate HLE. */
        core->busWrite16(core, 0x0203e000, (uint16_t)(0xdf00 | swi_number));
        core->busWrite16(core, 0x0203e002, 0x4770);
        if (rom_call(core, 0x0203e000, 0x02000000, 0x02001000, control)) return 1;
        for (i = 0; i < 80; ++i) if (actual[i] != core->busRead32(core, 0x02001000 + i * 4)) {
            fprintf(stderr, "BIOS copy variant %u count %u word %u differs\n", variant, count, i); fprintf(stderr, "actual %08x expected %08x source %08x\n", actual[i], core->busRead32(core, 0x02001000 + i * 4), core->busRead32(core, 0x02000000 + i * 4)); return 1;
        }
        ++cases;
    }
    for (variant = 1; variant <= 7; ++variant) for (count = 1; count <= 32; count += 3) {
        for (i = 0; i < 80; ++i) {
            actual[i] = source[i];
            core->busWrite32(core, 0x02000000 + i * 4, source[i]);
        }
        CpuFastSet(actual + 16, actual + 16 + variant, count);
        core->busWrite16(core, 0x0203e000, 0xdf0c);
        core->busWrite16(core, 0x0203e002, 0x4770);
        if (rom_call(core, 0x0203e000, 0x02000040, 0x02000040 + variant * 4, count)) return 1;
        for (i = 0; i < 80; ++i) if (actual[i] != core->busRead32(core, 0x02000000 + i * 4)) {
            fprintf(stderr, "CpuFastSet overlap %u count %u word %u differs\n", variant, count, i); return 1;
        }
        ++cases;
    }
    {
        static const struct { unsigned swi, size; uint8_t bytes[16]; } streams[] = {
            {0x13, 12, {0x28,4,0,0,1,0xc0,'A','B',0,0,0,0x60}},
            {0x13, 12, {0x24,4,0,0,1,0xc0,3,12,0,0,0,0x66}},
            {0x14, 9, {0x30,8,0,0,1,'a','b',0x83,'c'}},
            {0x16, 8, {0x81,4,0,0,250,10,2,255}},
            {0x18, 8, {0x82,4,0,0,255,255,2,0}}
        };
        for (variant = 0; variant < sizeof(streams) / sizeof(*streams); ++variant) {
            uint8_t decoded[16]; size_t written;
            for (i = 0; i < streams[variant].size; ++i) core->busWrite8(core, 0x02000000 + i, streams[variant].bytes[i]);
            if (hw_bios_decompress(streams[variant].swi, streams[variant].bytes, streams[variant].size, decoded, sizeof(decoded), &written)) return 1;
            swi(core, streams[variant].swi, 0x02000000, 0x02001000, 0, 0);
            for (i = 0; i < written; ++i) if (decoded[i] != core->busRead8(core, 0x02001000 + i)) {
                fprintf(stderr, "BIOS stream %u byte %u differs\n", variant, i); return 1;
            }
            ++cases;
        }
    }
    printf("mGBA BIOS memory/arithmetic: PASS (%u cases: Sqrt, Div, copies, Huffman, RL, delta)\n", cases);
    return 0;
}

static int wave_bank_oracle(struct mCore *core)
{
    static Hw hw;
    unsigned bank, i;
    hw_init(&hw, NULL);
    hw_write16(&hw, HW_IO_BASE + 0x84, 0x80);
    core->busWrite16(core, HW_IO_BASE + 0x84, 0x80);
    for (bank = 0; bank < 2; ++bank) {
        hw_write16(&hw, HW_IO_BASE + 0x70, (uint16_t)(bank << 6));
        core->busWrite16(core, HW_IO_BASE + 0x70, (uint16_t)(bank << 6));
        for (i = 0; i < 16; ++i) {
            uint8_t value = (uint8_t)(bank * 100 + i * 3);
            hw_write8(&hw, HW_IO_BASE + 0x90 + i, value);
            core->busWrite8(core, HW_IO_BASE + 0x90 + i, value);
        }
    }
    for (bank = 0; bank < 2; ++bank) {
        hw_write16(&hw, HW_IO_BASE + 0x70, (uint16_t)(bank << 6));
        core->busWrite16(core, HW_IO_BASE + 0x70, (uint16_t)(bank << 6));
        for (i = 0; i < 16; ++i) if (hw_read8(&hw, HW_IO_BASE + 0x90 + i) != core->busRead8(core, HW_IO_BASE + 0x90 + i)) {
            fprintf(stderr, "Wave bank %u byte %u differs\n", bank, i); return 1;
        }
    }
    puts("mGBA wave RAM: PASS (both CPU-visible banks)");
    return 0;
}

static int64_t rtc_oracle_seconds = INT64_C(1709251199);
static int64_t rtc_oracle_now(void *user) { (void)user; return rtc_oracle_seconds; }
static void rtc_oracle_sample(struct mRTCSource *source) { (void)source; }
static time_t rtc_oracle_unix(struct mRTCSource *source) { (void)source; return (time_t)rtc_oracle_seconds; }
static struct mRTCSource rtc_fixed_source = {rtc_oracle_sample, rtc_oracle_unix, NULL, NULL};

static int rtc_sdk_oracle(struct mCore *core)
{
    struct GBA *gba = core->board;
    static Hw hw;
    HwHost host = {0};
    HwRtcRecord record = {0};
    unsigned i;
    setenv("TZ", "UTC", 1); tzset();
    gba->rtcSource = &rtc_fixed_source;
    GBAHardwareInitRTC(&gba->memory.hw);
    host.unix_time = rtc_oracle_now;
    hw_init(&hw, &host); hw_bind(&hw); Rtc_Initialize();
    if (rom_call(core, 0x08248970, 0, 0, 0) || rom_call(core, 0x08248a78, 0, 0, 0)) return 1;
    if (rom_call(core, 0x08248c70, 0x02003000, 0, 0) || !Rtc_ReadDateTime(&record)) return 1;
    for (i = 0; i < 7; ++i) if (((uint8_t *)&record)[i] != core->busRead8(core, 0x02003000 + i)) {
        fprintf(stderr, "RTC SDK byte %u: original %u / port %u\n", i, core->busRead8(core, 0x02003000 + i), ((uint8_t *)&record)[i]); return 1;
    }
    if (rom_call(core, 0x08248afc, 0x02003000, 0, 0) || !Rtc_ReadStatus(&record) || record.status != core->busRead8(core, 0x02003007)) return 1;
    for (i = 0; i < 24; ++i) {
        rtc_oracle_seconds = INT64_C(1709164800) + i * 3600;
        if (rom_call(core, 0x08248c70, 0x02003000, 0, 0) || !Rtc_ReadDateTime(&record) ||
            record.hour != core->busRead8(core, 0x02003004) || hw.rtc_data[4] != gba->memory.hw.rtc.time[4]) return 1;
    }
    record.status = 7;
    core->busWrite8(core, 0x02003007, 7);
    if (rom_call(core, 0x08248bc8, 0x02003000, 0, 0) || !Rtc_WriteStatus(&record) ||
        rom_call(core, 0x08248afc, 0x02003000, 0, 0) || !Rtc_ReadStatus(&record) ||
        record.status != core->busRead8(core, 0x02003007)) return 1;
    if (rom_call(core, 0x08248988, 0, 0, 0)) return 1;
    Rtc_Protect();
    if (rom_call(core, 0x08248c70, 0x02003000, 0, 0) || ((struct ARMCore *)core->cpu)->gprs[0] != 0 || Rtc_ReadDateTime(&record)) return 1;
    Rtc_Initialize();
    if (rom_call(core, 0x08248970, 0, 0, 0) || rom_call(core, 0x082489a0, 0, 0, 0) ||
        (uint8_t)((struct ARMCore *)core->cpu)->gprs[0] != Rtc_CheckAndReset()) return 1;
    hw_bind(NULL);
    puts("Original RTC SDK: PASS (24 UTC hours, date/status reads, status write, protection, probe)");
    return 0;
}

int main(int argc, char **argv)
{
    struct mCore *core;
    struct ARMCore *cpu;
    FILE *rom_file;
    uint8_t *rom, decoded[65536];
    long rom_size;
    unsigned tests = 0, assets = 0;
    int x, y;
    if (argc != 2) { fprintf(stderr, "usage: hw-mgba ROM\n"); return 2; }
    mLogSetDefaultLogger(&logger);
    core = mCoreFind(argv[1]);
    if (!core || !core->init(core)) return 2;
    mCoreInitConfig(core, NULL);
    if (!mCoreLoadFile(core, argv[1])) return 2;
    core->reset(core); cpu = core->cpu;
    for (x = -32768; x <= 32767; x += 17) {
        swi(core, 9, (uint32_t)x, 0, 0, 0);
        if ((int16_t)cpu->gprs[0] != ArcTan((int16_t)x)) {
            fprintf(stderr, "ArcTan(%d): mGBA %d / port %d\n", x, (int16_t)cpu->gprs[0], ArcTan((int16_t)x)); return 1;
        }
        ++tests;
    }
    for (x = -32768; x <= 32767; x += 997) for (y = -32768; y <= 32767; y += 991) {
        swi(core, 10, (uint32_t)x, (uint32_t)y, 0, 0);
        if ((uint16_t)cpu->gprs[0] != ArcTan2((int16_t)x, (int16_t)y)) {
            fprintf(stderr, "ArcTan2(%d,%d): mGBA %u / port %u\n", x, y, (uint16_t)cpu->gprs[0], ArcTan2((int16_t)x, (int16_t)y)); return 1;
        }
        ++tests;
    }
    for (x = 0; x < 256; ++x) {
        struct BgAffineSrcData src = {10000, -20000, -50, 70, 311, -529, (uint16_t)(x << 8)};
        struct BgAffineDstData dest;
        const uint8_t *bytes = (const uint8_t *)&src;
        unsigned j;
        for (j = 0; j < sizeof(src); ++j) core->busWrite8(core, 0x02000000 + j, bytes[j]);
        swi(core, 14, 0x02000000, 0x02000100, 1, 0);
        BgAffineSet(&src, &dest, 1);
        /* mGBA's HLE affine coefficients round differently from BIOS integer
         * shifts for negative products. Allow one coefficient unit, and the
         * propagated reference-coordinate error. Cardinal cases are exact in
         * the portable suite. */
        bytes = (const uint8_t *)&dest;
        for (j = 0; j < 4; ++j) {
            int a = (int16_t)core->busRead16(core, 0x02000100 + j * 2);
            int b = (int16_t)(bytes[j * 2] | bytes[j * 2 + 1] << 8);
            if (abs(a - b) > 1) { fprintf(stderr, "Affine coefficient angle %d differs: %d / %d\n", x, a, b); return 1; }
        }
        for (j = 0; j < 2; ++j) {
            int32_t a = (int32_t)core->busRead32(core, 0x02000108 + j * 4);
            int32_t b = j ? dest.dy : dest.dx;
            if (abs(a - b) > 120) { fprintf(stderr, "Affine reference angle %d differs: %d / %d\n", x, a, b); return 1; }
        }
        ++tests;
    }
    if (wave_bank_oracle(core)) return 1;
    if (bios_memory_oracle(core)) return 1;
    if (mixer_oracle(core)) return 1;
    if (mixer_state_oracle(core)) return 1;
    if (mixer_reverb_oracle(core)) return 1;
    if (eeprom_oracle(core)) return 1;
    if (rtc_sdk_oracle(core)) return 1;
    rom_file = fopen(argv[1], "rb");
    if (!rom_file || fseek(rom_file, 0, SEEK_END)) return 2;
    rom_size = ftell(rom_file); rewind(rom_file);
    if (rom_size <= 0) return 2;
    rom = malloc((size_t)rom_size);
    if (!rom || fread(rom, 1, (size_t)rom_size, rom_file) != (size_t)rom_size) return 2;
    fclose(rom_file);
    for (size_t off = 0; off + 8 < (size_t)rom_size && assets < 128; off += 4) {
        size_t size, written;
        if (rom[off] != 0x10) continue;
        size = (size_t)rom[off + 1] | (size_t)rom[off + 2] << 8 | (size_t)rom[off + 3] << 16;
        if (size < 32 || size > sizeof(decoded) || (size & 3)) continue;
        if (hw_bios_decompress(0x11, rom + off, (size_t)rom_size - off, decoded, sizeof(decoded), &written)) continue;
        swi(core, 0x11, 0x08000000 + (uint32_t)off, 0x02000000, 0, 0);
        for (size_t j = 0; j < size; ++j) if (decoded[j] != core->busRead8(core, 0x02000000 + (uint32_t)j)) {
            fprintf(stderr, "LZ77 at %08X byte %zu differs\n", 0x08000000 + (unsigned)off, j); return 1;
        }
        ++assets;
    }
    free(rom); core->deinit(core);
    if (!assets) { fprintf(stderr, "No compressed assets tested\n"); return 1; }
    printf("mGBA oracle: PASS (%u arithmetic/affine cases, %u runtime ROM LZ77 candidates)\n", tests, assets);
    return 0;
}
