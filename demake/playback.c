/* Drive a DMG cartridge with libmgba's public core interface.
 * Input: step <frames> <key mask>, frame <path>, read <address> <count>.
 * All coordinates and samples come from the emulated cartridge.
 */
#include <mgba/flags.h>
#include <mgba/core/core.h>
#include <mgba/core/config.h>
#include <mgba/core/log.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if __has_include(<mgba-util/image.h>)
#include <mgba-util/image.h>
typedef mColor color_t;
#define desiredVideoDimensions baseVideoSize
#endif

static void quiet(struct mLogger* log, int category, enum mLogLevel level,
                  const char* format, va_list args) {
    (void) log;
    (void) category;
    if (level & (mLOG_FATAL | mLOG_ERROR)) {
        vfprintf(stderr, format, args);
        fputc('\n', stderr);
    }
}

static unsigned channel(unsigned color, unsigned mask) {
    unsigned low = mask & -mask;
    return ((color & mask) / low) * 255 / (mask / low);
}

int main(int argc, char** argv) {
    if (argc != 2) return 2;
    struct mLogger logger = {quiet, NULL};
    mLogSetDefaultLogger(&logger);
    struct mCore* core = mCoreFind(argv[1]);
    if (!core || core->platform(core) != mPLATFORM_GB || !core->init(core)) return 3;
    mCoreInitConfig(core, NULL);
    mCoreConfigSetValue(&core->config, "gb.model", "DMG");
    mCoreConfigSetIntValue(&core->config, "gb.colors", 0);
    mCoreConfigSetIntValue(&core->config, "useBios", 0);
    core->loadConfig(core, &core->config);
    if (!mCoreLoadFile(core, argv[1])) return 4;
    color_t pixels[160 * 144] = {0};
    core->setVideoBuffer(core, pixels, 160);
    core->reset(core);
    unsigned width, height;
    core->desiredVideoDimensions(core, &width, &height);
    if (width != 160 || height != 144) return 5;
    char line[1024], command[16], path[960];
    unsigned n, mask;
    while (fgets(line, sizeof(line), stdin)) {
        if (sscanf(line, "%15s", command) != 1) continue;
        if (!strcmp(command, "step") && sscanf(line, "%*s %u %u", &n, &mask) == 2) {
            core->setKeys(core, mask);
            while (n--) core->runFrame(core);
        } else if (!strcmp(command, "frame") && sscanf(line, "%*s %959[^\n]", path) == 1) {
            FILE* out = fopen(path, "wb");
            if (!out) return 6;
            fprintf(out, "P6\n%u %u\n255\n", width, height);
            for (unsigned i = 0; i < width * height; ++i) {
                fputc(channel(pixels[i], M_COLOR_RED), out);
                fputc(channel(pixels[i], M_COLOR_GREEN), out);
                fputc(channel(pixels[i], M_COLOR_BLUE), out);
            }
            if (fclose(out)) return 7;
        } else if (!strcmp(command, "read") && sscanf(line, "%*s %x %u", &mask, &n) == 2) {
            printf("%04x ", mask);
            for (unsigned i = 0; i < n; ++i) printf("%02x", (unsigned)core->busRead8(core, mask + i));
            putchar('\n');
        } else return 8;
        fflush(stdout);
    }
    mCoreConfigDeinit(&core->config);
    core->deinit(core);
    return 0;
}
