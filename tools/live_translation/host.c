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
#include <stdlib.h>
#include <string.h>

enum Hook {
    BANK = 1,
    MENU,
    RESOURCE,
    PLAYER_NAME,
    TICKER,
    BG_LAYER,
    OBJ_FRAME,
    SUBSTITUTION,
    DIRECT_TEXT,
    HUD_LAYER,
    PATTERN_SET,
    ACTOR_TILES
};
typedef uint32_t (*Resolver)(unsigned hook, unsigned id, unsigned ptr, unsigned arg,
                             unsigned caller, unsigned scene, unsigned proc, unsigned frame);
/* Cache the immutable frame table, never the temporary address of a relocated
 * header. A game's widget can reuse that header storage for a different asset. */
struct FrameChoice {
    uint32_t glyphs, index, replacement;
};
static struct FrameChoice frame_cache[2048], actor_cache[2048];
static uint32_t actor_frame, actor_palette, active_task, active_callback;
static struct mCore *core;
static struct ARMCore *cpu;
static uint32_t pixels[240 * 160];
static Resolver resolve;
static uint32_t text_window, string_id, resource_type, resource_key, resource_caller, last_proc;
/* A rewind point inside the text lookup, before its return register changes.
 * Cartridge scratch is immutable across loads, just like the original ROM. */
static unsigned erase_x, erase_y, erase_w, erase_h, erase_frame;
struct Bookmark {
    void *state;
    unsigned id, proc, window, actor_frame, actor_palette, active_task, active_callback;
    unsigned erase_x, erase_y, erase_w, erase_h, erase_frame;
};
static struct Bookmark bookmarks[2];
int lt_bookmark(unsigned slot)
{
    if (slot >= 2)
        return 0;
    struct Bookmark *b = &bookmarks[slot];
    if (!b->state)
        b->state = malloc(core->stateSize(core));
    if (!b->state || !core->saveState(core, b->state))
        return 0;
    b->actor_frame = actor_frame;
    b->actor_palette = actor_palette;
    b->active_task = active_task;
    b->active_callback = active_callback;
    b->erase_x = erase_x;
    b->erase_y = erase_y;
    b->erase_w = erase_w;
    b->erase_h = erase_h;
    b->erase_frame = erase_frame;
    b->id = string_id;
    b->proc = last_proc;
    b->window = text_window;
    return 1;
}
int lt_rewind(unsigned slot)
{
    if (slot >= 2)
        return -1;
    struct Bookmark *b = &bookmarks[slot];
    if (!b->state || !core->loadState(core, b->state))
        return -1;
    actor_frame = b->actor_frame;
    actor_palette = b->actor_palette;
    active_task = b->active_task;
    active_callback = b->active_callback;
    erase_x = b->erase_x;
    erase_y = b->erase_y;
    erase_w = b->erase_w;
    erase_h = b->erase_h;
    erase_frame = b->erase_frame;
    string_id = b->id;
    last_proc = b->proc;
    text_window = b->window;
    if (slot == 1) {
        memset(frame_cache, 0, sizeof(frame_cache));
        memset(actor_cache, 0, sizeof(actor_cache));
    }
    return (int)core->frameCounter(core);
}
static void logger_fn(struct mLogger *l, int c, enum mLogLevel level, const char *fmt, va_list a)
{
    if (level & (mLOG_FATAL | mLOG_ERROR)) {
        vfprintf(stderr, fmt, a);
        fputc('\n', stderr);
    }
}
static struct mLogger logger = {.log = logger_fn};
static uint8_t lux(struct GBALuminanceSource *l)
{
    return 80;
}
static void sample(struct GBALuminanceSource *l)
{
}
static struct GBALuminanceSource light = {.readLuminance = lux, .sample = sample};

static int capture_audio;
static int16_t stereo[8192];
static unsigned nsamples, sample_rate;
static void sample_audio(struct mAVStream *s, int16_t left, int16_t right)
{
    if (capture_audio && nsamples < 4096) {
        stereo[nsamples * 2] = left;
        stereo[nsamples * 2 + 1] = right;
        nsamples++;
    }
}
static void audio_rate(struct mAVStream *s, unsigned rate)
{
    sample_rate = rate;
}
static struct mAVStream av = {.postAudioFrame = sample_audio, .audioRateChanged = audio_rate};
void lt_sound(int enabled)
{
    capture_audio = enabled;
    nsamples = 0;
}
unsigned lt_take_audio(int16_t *out)
{
    memcpy(out, stereo, nsamples * 4);
    unsigned n = nsamples;
    nsamples = 0;
    return n;
}
unsigned lt_rate(void)
{
    return sample_rate;
}

int lt_open(const char *rom, const char *battery, Resolver callback)
{
    mLogSetDefaultLogger(&logger);
    core = mCoreFind(rom);
    if (!core || !core->init(core))
        return 0;
    mCoreInitConfig(core, NULL);
    core->setVideoBuffer(core, pixels, 240);
    if (!mCoreLoadFile(core, rom))
        return 0;
    if (battery && !core->loadSave(core, VFileOpen(battery, O_RDWR)))
        return 0;
    core->rtc.override = RTC_FIXED;
    core->rtc.value = 1128150000000LL;
    core->setPeripheral(core, mPERIPH_GBA_LUMINANCE, &light);
    sample_rate = 32768;
    nsamples = capture_audio = 0;
    core->setAVStream(core, &av);
    core->reset(core);
    cpu = core->cpu;
    resolve = callback;
    active_task = active_callback = last_proc = 0;
    erase_w = erase_h = 0;
    memset(frame_cache, 0, sizeof(frame_cache));
    memset(actor_cache, 0, sizeof(actor_cache));
    return 1;
}

void lt_close(void)
{
    for (unsigned i = 0; i < 2; i++) {
        free(bookmarks[i].state);
        bookmarks[i].state = NULL;
    }
    if (core)
        core->deinit(core);
    core = NULL;
}
void lt_read(uint32_t addr, uint8_t *out, unsigned length)
{
    for (unsigned i = 0; i < length; i++)
        out[i] = core->rawRead8(core, addr + i, -1);
}
void lt_install(uint32_t addr, const uint8_t *src, unsigned length)
{
    /* Private, initially zero cartridge space, kept for the session. */
    if (addr < 0x08F10000 || addr >= 0x09000000 || length > 0x09000000 - addr)
        return;
    for (unsigned i = 0; i < length; i++)
        core->rawWrite8(core, addr + i, -1, src[i]);
}
const uint32_t *lt_pixels(void)
{
    return pixels;
}
static uint32_t request(unsigned hook, unsigned id, unsigned ptr, unsigned arg, unsigned caller)
{
    uint32_t replacement =
        resolve(hook, id, ptr, arg, caller & ~1u, core->rawRead16(core, 0x030039F0, -1), last_proc,
                core->frameCounter(core));
    return replacement ? replacement : ptr;
}
uint32_t lt_task(void)
{
    return active_task;
}
uint32_t lt_callback(void)
{
    return active_callback;
}
void lt_step(unsigned frames, unsigned keys)
{
    core->setKeys(core, keys);
    while (frames--) {
        uint32_t start = core->frameCounter(core);
        while (core->frameCounter(core) == start) {
            while (cpu->cycles >= cpu->nextEvent)
                cpu->irqh.processEvents(cpu);
            uint32_t pc = cpu->gprs[15] - (cpu->executionMode == MODE_THUMB ? 2 : 4);
            switch (pc) {
                case 0x0821A086: /* TaskSystem_RunFrame calling task->update(task) */
                case 0x0821A09C: /* or task->destroy(task) */
                    active_task = cpu->gprs[0];
                    active_callback = cpu->gprs[1];
                    break;
                case 0x0821A08A:
                case 0x0821A0A0:
                    active_task = active_callback = 0;
                    break;
                case 0x08032CD4: /* TextState_SetString(state, index, string) */
                    if (cpu->gprs[2])
                        cpu->gprs[2] =
                            request(SUBSTITUTION, cpu->gprs[1], cpu->gprs[2], 0, cpu->gprs[14]);
                    break;
                case 0x08030EDC: /* MessageWindow_SetTextCursor(string) */
                    if (cpu->gprs[0])
                        cpu->gprs[0] = request(DIRECT_TEXT, 0, cpu->gprs[0], 0, cpu->gprs[14]);
                    break;
                case 0x080335F4:
                    text_window = cpu->gprs[0];
                    break; /* TextWindow_ResolveString */
                case 0x0803343C:
                    if (cpu->gprs[0])
                        cpu->gprs[0] = request(TICKER, 0, cpu->gprs[0], 0, cpu->gprs[14]);
                    break;
                case 0x0821AD08:
                    last_proc = cpu->gprs[0];
                    break; /* Script_Call */
                case 0x0821ADD4:
                    string_id = cpu->gprs[0];
                    break; /* Text_LookupString */
                case 0x0821ADF8:
                    cpu->gprs[0] = request(BANK, string_id, cpu->gprs[0],
                                           (cpu->gprs[14] & ~1u) == 0x08033604 ? text_window : 0,
                                           cpu->gprs[14]);
                    break;
                case 0x0803242A:
                case 0x0803292E: /* <NAME> assigned to the text interpreter's stream */
                    cpu->gprs[0] = request(PLAYER_NAME, 0, cpu->gprs[0], 0, pc);
                    break;
                case 0x081DC02C: /* Menu_EraseRect(x, y, width, height) */
                    erase_x = cpu->gprs[0];
                    erase_y = cpu->gprs[1];
                    erase_w = cpu->gprs[2];
                    erase_h = cpu->gprs[3];
                    erase_frame = start;
                    break;
                case 0x081DC054: { /* Menu_DrawText(column, row, final string) */
                    unsigned box = cpu->gprs[1] & 255;
                    if (start == erase_frame && cpu->gprs[0] == erase_x &&
                        cpu->gprs[1] == erase_y && erase_w <= 30 && erase_h <= 20)
                        box |= (erase_w << 8) | (erase_h << 16);
                    cpu->gprs[2] = request(MENU, cpu->gprs[0], cpu->gprs[2], box, cpu->gprs[14]);
                    break;
                }
                case 0x08214600: { /* Sprite_QueueFramePieces(sprite, x, y, z) */
                    uint32_t p = cpu->gprs[0];
                    uint32_t resource = core->rawRead32(core, p + 12, -1);
                    int index = (int16_t)core->rawRead16(core, p + 16, -1);
                    actor_frame = core->rawRead32(core, resource + 20, -1) + index * 12;
                    actor_palette = core->rawRead32(core, resource + 12, -1);
                    break;
                }
                case 0x082152E4: { /* ObjGfx_AllocateTileSlot(src, count), after on-screen culling */
                    uint32_t original = cpu->gprs[0];
                    struct FrameChoice *slot =
                        &actor_cache[((actor_frame >> 2) ^ (actor_palette >> 5)) % 2048];
                    if (slot->glyphs != actor_frame || slot->index != actor_palette) {
                        slot->glyphs = actor_frame;
                        slot->index = actor_palette;
                        slot->replacement = request(ACTOR_TILES, actor_frame, original,
                                                    actor_palette, cpu->gprs[14]);
                    }
                    cpu->gprs[0] = slot->replacement;
                    break;
                }
                case 0x08219728: { /* AnimSprite_SelectGlyph(widget, header, frame) */
                    uint32_t header = cpu->gprs[1], index = (uint16_t)cpu->gprs[2];
                    uint32_t glyphs = core->rawRead32(core, header + 12, -1);
                    struct FrameChoice *slot = &frame_cache[((glyphs >> 3) ^ index) % 2048];
                    if (glyphs < 0x08000000 || glyphs >= 0x09000000)
                        break;
                    if (slot->glyphs != glyphs || slot->index != index) {
                        slot->glyphs = glyphs;
                        slot->index = index;
                        slot->replacement =
                            request(OBJ_FRAME, index, header, glyphs, cpu->gprs[14]);
                        if (slot->replacement == header)
                            slot->replacement = 0;
                    }
                    if (slot->replacement) {
                        cpu->gprs[1] = slot->replacement;
                        cpu->gprs[2] = 0; /* Transient resource contains the selected frame only. */
                    }
                    break;
                }
                case 0x08215D40: { /* Bg_UploadTileEntries(layer, ptn, count, ids, first) */
                    unsigned count = cpu->gprs[2];
                    if (count && count <= 64) {
                        unsigned first = core->rawRead32(core, cpu->gprs[13], -1) & 1023;
                        unsigned packed = (count << 16) | (first << 2) | (cpu->gprs[0] & 3);
                        request(PATTERN_SET, packed, cpu->gprs[1], cpu->gprs[3], cpu->gprs[14]);
                    }
                    break;
                }
                case 0x08163F14: /* Hud_SetupBackgroundLayer(hud, map, offset, layer, palette) */
                    cpu->gprs[1] = request(HUD_LAYER, cpu->gprs[3], cpu->gprs[1],
                                           core->rawRead32(core, cpu->gprs[13], -1), cpu->gprs[14]);
                    break;
                case 0x082161B4: { /* Bg_SetupLayers(mode, id, map, x, y, count, maps) */
                    uint32_t sp = cpu->gprs[13];
                    uint32_t count = core->rawRead32(core, sp + 4, -1);
                    uint32_t maps = core->rawRead32(core, sp + 8, -1);
                    uint32_t layer = count && maps ? core->rawRead32(core, maps, -1) : 0;
                    cpu->gprs[2] =
                        request(BG_LAYER, layer, cpu->gprs[2], cpu->gprs[0], cpu->gprs[14]);
                    break;
                }
                case 0x0821A520: /* ScriptProc_Start(type, hash) */
                    resource_type = (uint16_t)cpu->gprs[0];
                    resource_key = (uint16_t)cpu->gprs[1];
                    resource_caller = cpu->gprs[14];
                    break;
                case 0x0821A65E:
                    if ((resource_type == 0xCB05 || resource_type == 0xC091 ||
                         resource_type == 0xCEEF || resource_type == 0x98F5) &&
                        cpu->gprs[0])
                        cpu->gprs[0] = request(RESOURCE, resource_key, cpu->gprs[0], resource_type,
                                               resource_caller);
                    break;
            }
            core->step(core);
        }
    }
}
