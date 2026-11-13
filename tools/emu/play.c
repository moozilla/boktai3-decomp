// Persistent mGBA host for Studio Play. No game or ROM-derived data lives here.
// Protocol: one ASCII command per line on stdin. Each reply is a JSON line,
// then RGB888, EWRAM, IWRAM, OAM, IO and interleaved signed-16 stereo audio.
// Sizes are in the JSON header; stdout is exclusively this protocol.
// Usage: play-host ROM PRIVATE_BATTERY FUNCTION_ADDRESSES
// Commands: step FRAMES KEYMASK TRACE LUX; snap; rewind FRAMES;
//           poke ADDRESS WIDTH VALUE; quit. All numbers are decimal.
#include <mgba/core/core.h>
#include <mgba/core/config.h>
#include <mgba/core/log.h>
#include <mgba/gba/interface.h>
#include <mgba/internal/arm/arm.h>
#include <mgba-util/vfs.h>
#if __has_include(<mgba-util/image.h>)
#include <mgba-util/image.h>
typedef mColor color_t;
#define desiredVideoDimensions baseVideoSize
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>

#define MAX_FUNCTIONS 16384
#define HASH_SIZE 32768
#define HISTORY 120
#define HISTORY_STRIDE 6
#define MAX_SAMPLES 32768
static struct mCore *core;
static struct ARMCore *cpu;
static color_t *pixels;
static unsigned width, height;
static uint32_t addresses[MAX_FUNCTIONS], counts[MAX_FUNCTIONS], hash[HASH_SIZE];
static unsigned nfunctions;
static uint32_t text_pc, text_id = UINT32_MAX, text_frame;
static uint8_t lux = 232;
static int16_t samples[MAX_SAMPLES * 2];
static unsigned nsamples, sample_rate = 32768;
static int capture_audio;
struct Snapshot {
    void *state;
    color_t *pixels;
    uint32_t frame, text_id, text_frame;
};
static struct Snapshot history[HISTORY];
static unsigned history_start, history_count;
static size_t state_size;

static void quiet_log(struct mLogger *l, int c, enum mLogLevel level, const char *fmt, va_list args) {
    (void)l; (void)c;
    if (level & (mLOG_FATAL | mLOG_ERROR)) { vfprintf(stderr, fmt, args); fputc('\n', stderr); }
}
static struct mLogger logger = {quiet_log, NULL};
static uint8_t read_lux(struct GBALuminanceSource *s) { (void)s; return lux; }
static void sample_lux(struct GBALuminanceSource *s) { (void)s; }
static struct GBALuminanceSource luminance = {sample_lux, read_lux};
static void audio_frame(struct mAVStream *s, int16_t left, int16_t right) {
    (void)s;
    if (capture_audio && nsamples < MAX_SAMPLES) {
        samples[2 * nsamples] = left; samples[2 * nsamples + 1] = right; ++nsamples;
    }
}
static void audio_rate(struct mAVStream *s, unsigned rate) { (void)s; sample_rate = rate; }
static struct mAVStream stream = {.audioRateChanged = audio_rate, .postAudioFrame = audio_frame};
static unsigned hash_pc(uint32_t pc) { return ((pc >> 1) * 2654435761u) & (HASH_SIZE - 1); }
static void note_exec(uint32_t pc) {
    unsigned slot = hash_pc(pc);
    while (hash[slot]) {
        unsigned index = hash[slot] - 1;
        if (addresses[index] == pc) { ++counts[index]; break; }
        slot = (slot + 1) & (HASH_SIZE - 1);
    }
    if (pc == text_pc) { text_id = (uint32_t)cpu->gprs[0] & 0x7FFFFFFF; text_frame = core->frameCounter(core); }
}
static int load_functions(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    unsigned address, is_text;
    while (fscanf(f, "%x %u", &address, &is_text) == 2) {
        if (nfunctions == MAX_FUNCTIONS) { fclose(f); return 0; }
        addresses[nfunctions] = address;
        unsigned slot = hash_pc(address);
        while (hash[slot]) slot = (slot + 1) & (HASH_SIZE - 1);
        hash[slot] = ++nfunctions;
        if (is_text) text_pc = address;
    }
    fclose(f);
    return nfunctions > 0;
}
static void remember(void) {
    unsigned slot;
    if (history_count == HISTORY) {
        slot = history_start; history_start = (history_start + 1) % HISTORY;
    } else slot = (history_start + history_count++) % HISTORY;
    struct Snapshot *s = &history[slot];
    if (!s->state) { s->state = malloc(state_size); s->pixels = malloc(width * height * sizeof(color_t)); }
    if (!s->state || !s->pixels || !core->saveState(core, s->state)) { fprintf(stderr, "savestate failed\n"); exit(1); }
    memcpy(s->pixels, pixels, width * height * sizeof(color_t));
    s->frame = core->frameCounter(core); s->text_id = text_id; s->text_frame = text_frame;
}
static int rewind_frames(unsigned frames) {
    if (!history_count) return 0;
    uint32_t now = core->frameCounter(core), target = now > frames ? now - frames : 0;
    unsigned chosen = 0;
    for (unsigned i = 0; i < history_count; ++i) {
        struct Snapshot *s = &history[(history_start + i) % HISTORY];
        if (s->frame <= target) chosen = i;
    }
    struct Snapshot *s = &history[(history_start + chosen) % HISTORY];
    if (!core->loadState(core, s->state)) return 0;
    memcpy(pixels, s->pixels, width * height * sizeof(color_t));
    text_id = s->text_id; text_frame = s->text_frame;
    history_count = chosen + 1;
    core->setKeys(core, 0);
    memset(counts, 0, sizeof(counts));
    return 1;
}
static void run_frames(unsigned n, unsigned keys, int trace) {
    core->setKeys(core, keys);
    for (unsigned i = 0; i < n; ++i) {
        // Coverage deliberately describes only the last frame in a request.
        memset(counts, 0, sizeof(counts));
        if (!trace) core->runFrame(core);
        else {
            uint32_t frame = core->frameCounter(core);
            while (core->frameCounter(core) == frame) {
                // Drain events before sampling: IRQs may redirect the next PC.
                while (cpu->cycles >= cpu->nextEvent) cpu->irqh.processEvents(cpu);
                uint32_t pc = cpu->gprs[15] - (cpu->executionMode == MODE_THUMB ? 2 : 4);
                note_exec(pc);
                core->step(core);
            }
        }
        if (core->frameCounter(core) % HISTORY_STRIDE == 0) remember();
    }
    core->setKeys(core, 0);
}
static void write_memory(uint32_t base, unsigned length) {
    uint8_t bytes[4096];
    for (unsigned offset = 0; offset < length; offset += sizeof(bytes)) {
        unsigned n = length - offset; if (n > sizeof(bytes)) n = sizeof(bytes);
        for (unsigned i = 0; i < n; ++i) bytes[i] = core->rawRead8(core, base + offset + i, -1);
        fwrite(bytes, 1, n, stdout);
    }
}
static void reply(int ok, int traced) {
    unsigned available = history_count ? core->frameCounter(core) - history[history_start].frame : 0;
    printf("{\"ok\":%s,\"frame\":%u,\"width\":%u,\"height\":%u,\"rgb\":%u,\"ram\":262144,\"iwram\":32768,\"oam\":1024,\"io\":1024,\"audio\":%u,\"rate\":%u,\"rewind\":%u,\"trace\":%s,\"text\":%u,\"text_frame\":%u,\"pc\":%u,\"hits\":[",
           ok ? "true" : "false", core->frameCounter(core), width, height, width * height * 3,
           nsamples * 4, sample_rate, available, traced ? "true" : "false", text_id, text_frame,
           cpu->gprs[15] - (cpu->executionMode == MODE_THUMB ? 2 : 4));
    int first = 1;
    for (unsigned i = 0; i < nfunctions; ++i) if (counts[i]) {
        printf("%s[%u,%u]", first ? "" : ",", addresses[i], counts[i]); first = 0;
    }
    printf("]}\n");
    uint8_t rgb[240 * 160 * 3];
    for (unsigned i = 0; i < width * height; ++i) {
        rgb[3 * i] = pixels[i] & 255; rgb[3 * i + 1] = (pixels[i] >> 8) & 255; rgb[3 * i + 2] = (pixels[i] >> 16) & 255;
    }
    fwrite(rgb, 1, width * height * 3, stdout);
    write_memory(0x02000000, 0x40000); write_memory(0x03000000, 0x8000);
    write_memory(0x07000000, 0x400); write_memory(0x04000000, 0x400);
    fwrite(samples, 4, nsamples, stdout);
    fflush(stdout);
}
int main(int argc, char **argv) {
    if (argc != 4) { fprintf(stderr, "usage: play-host ROM PRIVATE_BATTERY FUNCTION_ADDRESSES\n"); return 1; }
    mLogSetDefaultLogger(&logger);
    if (!load_functions(argv[3])) { fprintf(stderr, "cannot load function map\n"); return 1; }
    core = mCoreFind(argv[1]);
    if (!core || !core->init(core)) { fprintf(stderr, "cannot initialize mGBA\n"); return 1; }
    mCoreInitConfig(core, NULL);
    core->desiredVideoDimensions(core, &width, &height);
    if (width != 240 || height != 160) return 1;
    pixels = calloc(width * height, sizeof(color_t));
    if (!pixels || !mCoreLoadFile(core, argv[1])) return 1;
    core->setVideoBuffer(core, pixels, width);
    if (strcmp(argv[2], "-") && !core->loadSave(core, VFileOpen(argv[2], O_RDWR))) return 1;
    core->setPeripheral(core, mPERIPH_GBA_LUMINANCE, &luminance);
    core->rtc.override = RTC_FIXED; core->rtc.value = 1128150000LL * 1000;
    core->reset(core);
    core->setAVStream(core, &stream);
    cpu = core->cpu; state_size = core->stateSize(core);
    // The first command provides the first response. Startup never writes stdout.
    char line[256];
    while (fgets(line, sizeof(line), stdin)) {
        char cmd[16] = ""; unsigned a = 0, b = 0, c = 0, d = lux;
        int fields = sscanf(line, "%15s %u %u %u %u", cmd, &a, &b, &c, &d);
        int ok = 1, traced = 0; nsamples = 0;
        if (!strcmp(cmd, "quit")) break;
        if (!strcmp(cmd, "step") && fields == 5 && a <= 600 && b <= 1023 && c <= 1 && d <= 255) {
            capture_audio = a <= 12; lux = d; run_frames(a, b, c); capture_audio = 0; traced = c;
        } else if (!strcmp(cmd, "snap") && fields == 1) { /* snapshot only */ }
        else if (!strcmp(cmd, "rewind") && fields == 2 && a) ok = rewind_frames(a);
        else if (!strcmp(cmd, "poke") && fields == 4 && (b == 1 || b == 2 || b == 4) && a % b == 0 &&
                 a >= 0x02000000 && a <= 0x02040000 - b) {
            if (b == 1) core->busWrite8(core, a, c);
            else if (b == 2) core->busWrite16(core, a, c);
            else core->busWrite32(core, a, c);
            remember();
        } else ok = 0;
        reply(ok, traced);
    }
    for (unsigned i = 0; i < HISTORY; ++i) { free(history[i].state); free(history[i].pixels); }
    core->deinit(core); free(core); free(pixels);
    return 0;
}
