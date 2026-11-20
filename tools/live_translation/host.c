/* Observe source-level ABI boundaries in the Japanese cartridge. Translation
 * policy lives in the host callback, and all drawing is performed by the GBA. */
#include <mgba/core/core.h>
#include <mgba/core/config.h>
#include <mgba/core/log.h>
#include <mgba/gba/interface.h>
#include <mgba/internal/arm/arm.h>
#include <mgba-util/vfs.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum Hook { BANK = 1, MENU, RESOURCE, PLAYER_NAME };
typedef uint32_t (*Resolver)(unsigned hook, unsigned id, unsigned ptr, unsigned arg,
                             unsigned caller, unsigned scene, unsigned proc, unsigned frame);
static struct mCore *core;
static struct ARMCore *cpu;
static uint32_t pixels[240 * 160];
static Resolver resolve;
static uint32_t string_id, resource_type, resource_key, resource_caller, last_proc;
static void logger_fn(struct mLogger *l, int c, enum mLogLevel level, const char *fmt, va_list a)
{
    if (level & (mLOG_FATAL | mLOG_ERROR)) { vfprintf(stderr, fmt, a); fputc('\n', stderr); }
}
static struct mLogger logger = {.log = logger_fn};
static uint8_t lux(struct GBALuminanceSource *l) { return 80; }
static void sample(struct GBALuminanceSource *l) {}
static struct GBALuminanceSource light = {.readLuminance = lux, .sample = sample};

int lt_open(const char *rom, const char *battery, Resolver callback)
{
    mLogSetDefaultLogger(&logger);
    core = mCoreFind(rom);
    if (!core || !core->init(core)) return 0;
    mCoreInitConfig(core, NULL);
    core->setVideoBuffer(core, pixels, 240);
    if (!mCoreLoadFile(core, rom)) return 0;
    if (battery && !core->loadSave(core, VFileOpen(battery, O_RDWR))) return 0;
    core->rtc.override = RTC_FIXED;
    core->rtc.value = 1128150000000LL;
    core->setPeripheral(core, mPERIPH_GBA_LUMINANCE, &light);
    core->reset(core);
    cpu = core->cpu;
    resolve = callback;
    last_proc = 0;
    return 1;
}

void lt_close(void) { if (core) core->deinit(core); core = NULL; }
void lt_read(uint32_t addr, uint8_t *out, unsigned length)
{
    for (unsigned i = 0; i < length; i++) out[i] = core->rawRead8(core, addr + i, -1);
}
void lt_install(uint32_t addr, const uint8_t *src, unsigned length)
{
    /* Private, initially zero cartridge space, kept for the session. */
    if (addr < 0x08F10000 || addr >= 0x09000000 || length > 0x09000000 - addr) return;
    for (unsigned i = 0; i < length; i++) core->rawWrite8(core, addr + i, -1, src[i]);
}
const uint32_t *lt_pixels(void) { return pixels; }
static uint32_t request(unsigned hook, unsigned id, unsigned ptr, unsigned arg, unsigned caller)
{
    uint32_t replacement = resolve(hook, id, ptr, arg, caller & ~1u,
        core->rawRead16(core, 0x030039F0, -1), last_proc, core->frameCounter(core));
    return replacement ? replacement : ptr;
}
void lt_step(unsigned frames, unsigned keys)
{
    core->setKeys(core, keys);
    while (frames--) {
        uint32_t start = core->frameCounter(core);
        while (core->frameCounter(core) == start) {
            while (cpu->cycles >= cpu->nextEvent) cpu->irqh.processEvents(cpu);
            uint32_t pc = cpu->gprs[15] - (cpu->executionMode == MODE_THUMB ? 2 : 4);
            switch (pc) {
            case 0x0821AD08: last_proc = cpu->gprs[0]; break; /* Script_Call */
            case 0x0821ADD4: string_id = cpu->gprs[0]; break; /* Text_LookupString */
            case 0x0821ADF8:
                cpu->gprs[0] = request(BANK, string_id, cpu->gprs[0], 0, cpu->gprs[14]);
                break;
            case 0x0803242A: case 0x0803292E: /* <NAME> assigned to the text interpreter's stream */
                cpu->gprs[0] = request(PLAYER_NAME, 0, cpu->gprs[0], 0, pc);
                break;
            case 0x081DC054: /* Menu_DrawText(column, row, final string) */
                cpu->gprs[2] = request(MENU, cpu->gprs[0], cpu->gprs[2], cpu->gprs[1], cpu->gprs[14]);
                break;
            case 0x0821A520: /* ScriptProc_Start(type, hash) */
                resource_type = (uint16_t)cpu->gprs[0];
                resource_key = (uint16_t)cpu->gprs[1];
                resource_caller = cpu->gprs[14];
                break;
            case 0x0821A65E:
                if (resource_type == 0xCB05 && cpu->gprs[0])
                    cpu->gprs[0] = request(RESOURCE, resource_key, cpu->gprs[0], resource_type, resource_caller);
                break;
            }
            core->step(core);
        }
    }
}
