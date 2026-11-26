// Headless mGBA harness for Boktai 3 reverse engineering.
//
// Runs the ROM from a command script and records:
//   * screenshots (PPM) on demand
//   * code coverage: every executed ROM/IWRAM/EWRAM address, with ARM/Thumb mode
//   * data coverage: every ROM byte read by a load (including DMA), with the
//     PC of the first instruction that read it
//
// Usage: harness ROM SCRIPT OUTDIR
//
// Script commands (one per line, # comments):
//   wait N              run N frames with no keys held
//   hold KEYS N         hold keys (e.g. A, START, A+UP) for N frames
//   tap KEYS            hold for 2 frames, release for 8
//   shot NAME           write OUTDIR/NAME.ppm
//   save NAME / load NAME   savestate to/from OUTDIR/NAME.ss
//   record PATH         start recording video and audio (mGBA's FFmpeg
//                       encoder, H.264/AAC in MP4, 4x scaled; needs an mGBA
//                       built with FFmpeg); "stoprecord" ends it (so does exit)
//   loadstate PATH      load an mGBA savestate (raw or PNG, e.g. one Centaur
//                       wrote); PATH relative to the script's directory
//   dump NAME ADDR:LEN  raw memory dump (hex address and length)
//   watch ADDR:LEN      log stores into a RAM range (hex) to stderr
//   poke ADDR[:SIZE] V  write V (hex) to memory once (SIZE 1/2/4, default 4)
//   freeze ADDR[:SIZE] V  same, rewritten before every frame; "unfreeze" clears
//   lux N               solar sensor reading (0-255, game treats lower as brighter)
//   rtc SECONDS         set the cartridge clock (unix seconds, UTC)
//   trace on|off        enable coverage recording (default on)
//   probe NAME ADDR     log registers before executing ADDR (hex) to probes.tsv
//   watchpc ADDR LO HI  arm a latch: set when ADDR (hex) executes with r0 in
//                       [LO, HI] (decimal), e.g. Text_LookupString of a string
//   tick ADDR           the function holdt counts (hex), e.g. the game's input read
//   fasttick ADDR       count at a Thumb literal load at entry or immediately after PUSH.
//                       Samples can differ from tick in the stack written by PUSH.
//   tickdump NAME FROM:TO ADDR:LEN[,ADDR+OFFSET:LEN,...]  append the regions to OUTDIR/NAME.bin at the
//                       entry to each selected tick. ADDR+OFFSET follows a RAM pointer.
//                       entry of each tick FROM..TO-1 (counted from "tick"; decimal)
//   holdt KEYS N        hold keys (NONE for none) until ADDR has run N more times:
//                       inputs keyed to game ticks, so lag frames do not shift them
//   waitpc MAX [N]      run until the armed latch is set (N times, default 1; at most MAX frames;
//                       the rest of that frame runs too), then disarm it
//   unprobe             remove all probes; probes also work with trace off
//   (env BOKTAI3_SAV=file.sav loads a battery save first)
//   mark NAME           start a new coverage segment: subsequent coverage is
//                       written to OUTDIR/cov-NAME.* in addition to the totals

#include <mgba/core/core.h>
#include <mgba/core/log.h>
#include <mgba/core/config.h>
#include <mgba/core/serialize.h>
#include <mgba/gba/interface.h>
#include <mgba/internal/arm/arm.h>
#include <mgba-util/vfs.h>
#if __has_include(<mgba-util/image.h>)  // mGBA 0.11 (tools/mgba/build_mgba.sh): savestates match Centaur's
#include <mgba-util/image.h>
typedef mColor color_t;
#define desiredVideoDimensions baseVideoSize
#endif

#ifdef USE_FFMPEG
#include "feature/ffmpeg/ffmpeg-encoder.h"
#endif
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <ctype.h>

#include "../studio/experiments/observe.h"
#include "../studio/experiments/draw_trace.h"

#define ROM_SIZE 0x1000000

static struct mCore* core;
#ifdef USE_FFMPEG
static struct FFmpegEncoder encoder;
static int recording;
static void stop_recording(void) {
	if (recording) { core->setAVStream(core, NULL); FFmpegEncoderClose(&encoder); recording = 0; }
}
#endif
static struct ARMCore* cpu;
static struct ARMMemory orig;
static color_t* fb;
static unsigned W, H;
static const char* outdir;
static int tracing = 1;

// Per-byte ROM data read: PC of first reader (0 = never read)
static uint32_t* rom_reader;
// Per-halfword code execution: bit0 = executed in Thumb, bit1 = executed in ARM
static uint8_t* rom_exec;
// Per-word: PC of the first LDR/LDM that loaded it (pointer evidence), 0 = none
static uint32_t* rom_word32;
static uint8_t iwram_exec[0x8000 / 2];
static uint8_t ewram_exec[0x40000 / 2];
// Segment (per "mark") copies
static uint32_t* seg_reader;
static uint8_t* seg_exec;
static char seg_name[256] = "";

static struct { char name[64]; uint32_t pc; } probes[32];
static unsigned nprobes;
static uint32_t probe_lo = UINT32_MAX, probe_hi;
static FILE* probe_file;
static unsigned probe_count;

static void add_probe(const char* name, const char* address) {
	char* end;
	unsigned long pc = strtoul(address, &end, 16);
	if (!*name || strlen(name) >= sizeof(probes[0].name) || !*address || *end ||
	    pc > UINT32_MAX || (pc & 1) || nprobes == 32) {
		fprintf(stderr, "invalid probe (use a name and an even hex address; at most 32)\n");
		exit(1);
	}
	for (const char* p = name; *p; ++p) {
		if (!isalnum((unsigned char)*p) && *p != '_' && *p != '-' && *p != '.') {
			fprintf(stderr, "probe names must use letters, digits, _, - or .\n");
			exit(1);
		}
	}
	if (!probe_file) {
		char path[1024];
		snprintf(path, sizeof(path), "%s/probes.tsv", outdir);
		probe_file = fopen(path, "w");
		if (!probe_file) { perror(path); exit(1); }
		fprintf(probe_file, "sequence\tframe\tsegment\tprobe\tpc\tmode");
		for (int i = 0; i < 16; ++i) fprintf(probe_file, "\tr%d", i);
		fputc('\n', probe_file);
	}
	strcpy(probes[nprobes].name, name);
	probes[nprobes++].pc = pc;
	if (pc < probe_lo) probe_lo = pc;
	if (pc > probe_hi) probe_hi = pc;
}

static bool note_probes(uint32_t pc) {
	if (pc < probe_lo || pc > probe_hi) return false;
	bool entered = false;
	for (unsigned i = 0; i < nprobes; ++i) {
		if (pc != probes[i].pc) continue;
		entered = true;
		// Fail explicitly instead of silently delivering a truncated trace.
		if (probe_count == 100000) {
			fprintf(stderr, "probe trace exceeded 100000 records; narrow the replay\n");
			exit(1);
		}
		fprintf(probe_file, "%u\t%u\t%s\t%s\t%08X\t%s", ++probe_count,
		        core->frameCounter(core), seg_name, probes[i].name, pc,
		        cpu->executionMode == MODE_THUMB ? "thumb" : "arm");
		for (int r = 0; r < 16; ++r) fprintf(probe_file, "\t%08X", (uint32_t)cpu->gprs[r]);
		fputc('\n', probe_file);
	}
	return entered;
}

static uint8_t lux = 0xE8;

static void quiet_log(struct mLogger* l, int cat, enum mLogLevel lvl, const char* fmt, va_list args) {
	(void) l; (void) cat;
	if (lvl & (mLOG_FATAL | mLOG_ERROR)) { vfprintf(stderr, fmt, args); fputc('\n', stderr); }
}
static struct mLogger quiet = { quiet_log, NULL };

static uint8_t readLux(struct GBALuminanceSource* s) { (void) s; return lux; }
static void sampleLux(struct GBALuminanceSource* s) { (void) s; }
static struct GBALuminanceSource luxsrc = { sampleLux, readLux };

static uint32_t cur_pc(void) {
	// gprs[15] is the prefetch address; subtract pipeline offset
	int thumb = cpu->executionMode == MODE_THUMB;
	return cpu->gprs[15] - (thumb ? 4 : 8);
}

static inline void note_read(uint32_t addr, int width) {
	if (!tracing) return;
	if (addr < 0x08000000 || addr >= 0x0E000000) return;
	uint32_t o = (addr - 0x08000000) & (ROM_SIZE - 1);
	uint32_t pc = cur_pc() | (cpu->executionMode == MODE_THUMB);
	for (int i = 0; i < width && o + i < ROM_SIZE; ++i) {
		if (!rom_reader[o + i]) rom_reader[o + i] = pc;
		if (seg_reader && !seg_reader[o + i]) seg_reader[o + i] = pc;
	}
}

// True if the current instruction is a word load (LDR/LDM), i.e. this access
// is the program loading a word -- not DMA or a BIOS copy.
static int cpu_is_word_load(void) {
	uint32_t pc = cur_pc();
	if (pc < 0x02000000) return 0; // BIOS (CpuSet/CpuFastSet/decompression)
	if (cpu->executionMode == MODE_THUMB) {
		uint16_t op = core->rawRead16(core, pc, -1);
		return (op & 0xF800) == 0x6800 || (op & 0xFE00) == 0x5800 || (op & 0xF800) == 0x4800 ||
		       (op & 0xF800) == 0x9800 || (op & 0xF800) == 0xC800 || (op & 0xFE00) == 0xBC00;
	}
	uint32_t op = core->rawRead32(core, pc, -1);
	return (op & 0x0C500000) == 0x04100000 || (op & 0x0E100000) == 0x08100000;
}

static inline void note_word(uint32_t addr, int nwords) {
	if (!tracing || addr < 0x08000000 || addr >= 0x0A000000) return;
	if (!cpu_is_word_load()) return;
	uint32_t w = ((addr - 0x08000000) & (ROM_SIZE - 1)) >> 2;
	uint32_t pc = cur_pc() | (cpu->executionMode == MODE_THUMB);
	for (int i = 0; i < nwords && w + i < ROM_SIZE / 4; ++i)
		if (!rom_word32[w + i]) rom_word32[w + i] = pc;
}
static uint32_t fast_tick_pc, fast_tick_address;
static int fast_tick_running;
static void count_tick(void);

static uint32_t h_load32(struct ARMCore* c, uint32_t a, int* cc) {
	if (fast_tick_running && a == fast_tick_address && cur_pc() == fast_tick_pc)
		count_tick();
	note_read(a & ~3u, 4);
	note_word(a & ~3u, 1);
	return orig.load32(c, a, cc);
}
static uint32_t h_load16(struct ARMCore* c, uint32_t a, int* cc) { note_read(a & ~1u, 2); return orig.load16(c, a, cc); }
static uint32_t h_load8(struct ARMCore* c, uint32_t a, int* cc) { note_read(a, 1); return orig.load8(c, a, cc); }
static uint32_t h_loadMultiple(struct ARMCore* c, uint32_t base, int mask, enum LSMDirection dir, int* cc) {
	int n = __builtin_popcount(mask & 0xFFFF);
	uint32_t start = base;
	if (dir & LSM_D) start = base - 4 * n + ((dir & LSM_B) ? 0 : 4);
	else if (dir & LSM_B) start = base + 4;
	note_read(start & ~3u, 4 * n);
	note_word(start & ~3u, n);
	return orig.loadMultiple(c, base, mask, dir, cc);
}

// Write watchpoint (watch ADDR:LEN): logs PC/value of stores into the range
static uint32_t watch_lo = 0, watch_hi = 0;
static void note_store(uint32_t a, uint32_t v, int w) {
	if (a + w > watch_lo && a < watch_hi)
		fprintf(stderr, "[watch] frame %u pc %08X store%d [%08X] = %08X\n", core->frameCounter(core), cur_pc(), w * 8, a, v);
}
static void h_store32(struct ARMCore* c, uint32_t a, int32_t v, int* cc) {
	bool observe = observe_before_store(a & ~3u, 4);
	note_store(a & ~3u, v, 4);
	if (a >= 0x040000D4 && a <= 0x040000DC && draw_trace_active())
		draw_trace_dma(a & ~3u, v, cur_pc());
	orig.store32(c, a, v, cc);
	if (observe) observe_after_store(cur_pc());
}
static void h_store16(struct ARMCore* c, uint32_t a, int16_t v, int* cc) {
	bool observe = observe_before_store(a & ~1u, 2);
	note_store(a & ~1u, (uint16_t)v, 2);
	orig.store16(c, a, v, cc);
	if (observe) observe_after_store(cur_pc());
}
static void h_store8(struct ARMCore* c, uint32_t a, int8_t v, int* cc) {
	bool observe = observe_before_store(a, 1);
	note_store(a, (uint8_t)v, 1);
	orig.store8(c, a, v, cc);
	if (observe) observe_after_store(cur_pc());
}
static uint32_t h_storeMultiple(struct ARMCore* c, uint32_t base, int mask, enum LSMDirection dir, int* cc) {
	int n = __builtin_popcount(mask & 0xFFFF);
	uint32_t start = base;
	if (dir & LSM_D) start = base - 4 * n + ((dir & LSM_B) ? 0 : 4);
	else if (dir & LSM_B) start = base + 4;
	bool observe = observe_before_store(start & ~3u, 4 * n);
	if (watch_hi && start < watch_hi && start + 4 * n > watch_lo)
		fprintf(stderr, "[watch] frame %u pc %08X stm [%08X..+%d]\n", core->frameCounter(core), cur_pc(), start, 4 * n);
	uint32_t result = orig.storeMultiple(c, base, mask, dir, cc);
	if (observe) observe_after_store(cur_pc());
	return result;
}

static void note_exec(uint32_t pc) {
	uint8_t bit = cpu->executionMode == MODE_THUMB ? 1 : 2;
	if (pc >= 0x08000000 && pc < 0x0A000000) {
		uint32_t o = (pc - 0x08000000) & (ROM_SIZE - 1);
		rom_exec[o >> 1] |= bit;
		if (seg_exec) seg_exec[o >> 1] |= bit;
	} else if (pc >= 0x03000000 && pc < 0x03008000) {
		iwram_exec[(pc - 0x03000000) >> 1] |= bit;
	} else if (pc >= 0x02000000 && pc < 0x02040000) {
		ewram_exec[(pc - 0x02000000) >> 1] |= bit;
	}
}

// "freeze" entries: rewritten before every frame
static struct { uint32_t addr, size, val; } freezes[32];
static int nfreeze;

static void poke(uint32_t addr, uint32_t size, uint32_t val) {
	if (size == 1) core->busWrite8(core, addr, val);
	else if (size == 2) core->busWrite16(core, addr, val);
	else core->busWrite32(core, addr, val);
}

// waitpc: the PC and r0 range to stop at; hit is set when it executes
static uint32_t until_pc;
static int32_t until_lo, until_hi;
static int until_hit;
// holdt: calls of tick_pc so far
static uint32_t tick_pc;
static int tick_hits;
// tickdump: absolute tick count since "tick", and regions dumped at the entry of
// ticks [tickdump_from, tickdump_to) (appended to OUTDIR/NAME.bin, one record per tick)
static long tick_total;
static long tickdump_from = -1, tickdump_to = -1;
static struct { uint32_t addr, offset, len; int indirect; } tickdump_r[32];
static int tickdump_n;
static FILE* tickdump_f;

static int tickdump_ram(uint32_t addr, uint32_t len) {
	return (addr >= 0x02000000 && addr < 0x02040000 && len <= 0x02040000 - addr) ||
	       (addr >= 0x03000000 && addr < 0x03008000 && len <= 0x03008000 - addr);
}

static void tickdump_hit(void) {
	if (!tickdump_f || tick_total < tickdump_from || tick_total >= tickdump_to) return;
	for (int r = 0; r < tickdump_n; ++r) {
		uint32_t addr = tickdump_r[r].addr, len = tickdump_r[r].len;
		int readable = 1;
		if (tickdump_r[r].indirect) {
			uint32_t ptr = core->rawRead32(core, addr, -1);
			addr = ptr + tickdump_r[r].offset;
			readable = tickdump_ram(ptr, 1) && addr >= ptr && tickdump_ram(addr, len);
		}
		for (uint32_t i = 0; i < len; ++i)
			fputc(readable ? core->rawRead8(core, addr + i, -1) : 0, tickdump_f);
	}
}

static void count_tick(void) {
	tickdump_hit();
	tick_hits++;
	tick_total++;
}

static int set_fast_tick(uint32_t entry) {
	uint32_t pc = entry;
	uint16_t opcode = core->rawRead16(core, pc, -1);
	if ((opcode & 0xfe00) == 0xb400) {
		pc += 2;
		opcode = core->rawRead16(core, pc, -1);
	}
	if ((opcode & 0xf800) != 0x4800) return 0;
	fast_tick_pc = pc;
	fast_tick_address = ((pc + 4) & ~3u) + (opcode & 0xff) * 4;
	return 1;
}

static void run_frames(int n) {
	for (int f = 0; f < n; ++f) {
		for (int i = 0; i < nfreeze; ++i) poke(freezes[i].addr, freezes[i].size, freezes[i].val);
		if (!tracing && !nprobes && !until_pc && !draw_trace_active() &&
		    (!tick_pc || fast_tick_pc)) {
			fast_tick_running = fast_tick_pc != 0;
			core->runFrame(core);
			fast_tick_running = 0;
			continue;
		}
		uint32_t frame = core->frameCounter(core);
		while (core->frameCounter(core) == frame) {
			// ARMRun processes events before executing an instruction. Drain
			// them before sampling: an IRQ can redirect PC, otherwise a probe
			// can report an instruction that has not actually executed yet.
			while (cpu->cycles >= cpu->nextEvent) cpu->irqh.processEvents(cpu);
			// Between steps PC points one instruction beyond the next one;
			// cur_pc() is instead for in-instruction memory hooks.
			uint32_t pc = cpu->gprs[15] - (cpu->executionMode == MODE_THUMB ? 2 : 4);
			if (tracing) note_exec(pc);
			if (draw_trace_active())
				draw_trace_step(pc, cpu->gprs[ARM_SP], cpu->gprs[ARM_LR], cpu->gprs[0], cpu->privilegeMode);
			if (nprobes) {
				observe_instruction(pc, cpu->gprs[ARM_SP], cpu->gprs[ARM_LR],
				                    cpu->privilegeMode, note_probes(pc));
			}
			if (until_pc && pc == until_pc && (int32_t)cpu->gprs[0] >= until_lo && (int32_t)cpu->gprs[0] <= until_hi)
				until_hit++;
			if (tick_pc && pc == tick_pc) count_tick();
			core->step(core);
		}
	}
}

static uint32_t parse_keys(const char* s) {
	static const struct { const char* n; uint32_t k; } map[] = {
		{"A", 1}, {"B", 2}, {"SELECT", 4}, {"START", 8}, {"RIGHT", 16}, {"LEFT", 32},
		{"UP", 64}, {"DOWN", 128}, {"R", 256}, {"L", 512}, {NULL, 0}};
	uint32_t keys = 0;
	char buf[128];
	strncpy(buf, s, sizeof(buf) - 1);
	buf[sizeof(buf) - 1] = 0;
	for (char* t = strtok(buf, "+"); t; t = strtok(NULL, "+")) {
		int found = 0;
		for (int i = 0; map[i].n; ++i) if (!strcmp(map[i].n, t)) { keys |= map[i].k; found = 1; }
		if (!found) { fprintf(stderr, "unknown key %s\n", t); exit(1); }
	}
	return keys;
}

static void shot(const char* name) {
	char path[1024];
	snprintf(path, sizeof(path), "%s/%s.ppm", outdir, name);
	FILE* f = fopen(path, "wb");
	fprintf(f, "P6\n%u %u\n255\n", W, H);
	for (unsigned i = 0; i < W * H; ++i) {
		color_t c = fb[i];
		// mGBA default color_t is 32-bit XBGR8
		uint8_t px[3] = { c & 0xFF, (c >> 8) & 0xFF, (c >> 16) & 0xFF };
		fwrite(px, 1, 3, f);
	}
	fclose(f);
}

static void write_cov(const char* tag, uint32_t* reader, uint8_t* exec) {
	char path[1024];
	snprintf(path, sizeof(path), "%s/%s.read", outdir, tag);
	FILE* f = fopen(path, "wb"); fwrite(reader, 4, ROM_SIZE, f); fclose(f);
	snprintf(path, sizeof(path), "%s/%s.exec", outdir, tag);
	f = fopen(path, "wb"); fwrite(exec, 1, ROM_SIZE / 2, f); fclose(f);
}

static void end_segment(void) {
	if (!seg_name[0]) return;
	char tag[160];
	snprintf(tag, sizeof(tag), "cov-%s", seg_name);
	write_cov(tag, seg_reader, seg_exec);
	seg_name[0] = 0;
}

static void savestate(const char* name, int load) {
	char path[1024];
	snprintf(path, sizeof(path), "%s/%s.ss", outdir, name);
	size_t size = core->stateSize(core);
	// mGBA leaves unscheduled event fields and reserved bytes unwritten.
	void* state = load ? malloc(size) : calloc(1, size);
	if (!state) { fprintf(stderr, "cannot allocate savestate buffer\n"); exit(1); }
	FILE* f = fopen(path, load ? "rb" : "wb");
	if (!f) { perror(path); exit(1); }
	if (load) {
		if (fread(state, 1, size, f) != size || fgetc(f) != EOF || !core->loadState(core, state)) {
			fprintf(stderr, "incompatible savestate %s\n", path);
			exit(1);
		}
	} else if (!core->saveState(core, state) || fwrite(state, 1, size, f) != size) {
		fprintf(stderr, "cannot write savestate %s\n", path);
		exit(1);
	}
	if (fclose(f)) { perror(path); exit(1); }
	free(state);
}

static char scriptdir[1024] = ".";

static void loadstate(const char* rel) {
	char path[2048];
	if (rel[0] == '/') snprintf(path, sizeof(path), "%s", rel);
	else snprintf(path, sizeof(path), "%s/%s", scriptdir, rel);
	struct VFile* vf = VFileOpen(path, O_RDONLY);
	if (!vf) { fprintf(stderr, "no state %s\n", path); exit(1); }
	if (!mCoreLoadStateNamed(core, vf, SAVESTATE_SAVEDATA | SAVESTATE_RTC)) {
		fprintf(stderr, "cannot load state %s (made by a newer mGBA?)\n", path); exit(1);
	}
	vf->close(vf);
}

int main(int argc, char** argv) {
	if (argc != 4) { fprintf(stderr, "usage: %s ROM SCRIPT OUTDIR\n", argv[0]); return 1; }
	outdir = argv[3];
	// mGBA's cartridge RTC converts the clock with localtime_r: pin the zone so
	// "rtc SECONDS" (and BOKTAI3_RTC) mean UTC on every host, as the port's clock does
	setenv("TZ", "UTC", 1);
	tzset();
	mLogSetDefaultLogger(&quiet);
	core = mCoreFind(argv[1]);
	if (!core || !core->init(core)) { fprintf(stderr, "core init failed\n"); return 1; }
	mCoreInitConfig(core, NULL);
	core->desiredVideoDimensions(core, &W, &H);
	fb = calloc(W * H, sizeof(color_t));
	core->setVideoBuffer(core, fb, W);
	if (!mCoreLoadFile(core, argv[1])) { fprintf(stderr, "load failed\n"); return 1; }
	core->setPeripheral(core, mPERIPH_GBA_LUMINANCE, &luxsrc);
	// Optional battery save: BOKTAI3_SAV=path (copied, never modified)
	const char* sav = getenv("BOKTAI3_SAV");
	if (sav) {
		char path[1024];
		snprintf(path, sizeof(path), "%s/battery.sav", outdir);
		FILE* in = fopen(sav, "rb");
		FILE* out = fopen(path, "wb");
		if (!in || !out) { fprintf(stderr, "cannot copy save %s\n", sav); return 1; }
		char buf[4096]; size_t n;
		while ((n = fread(buf, 1, sizeof(buf), in)) > 0) fwrite(buf, 1, n, out);
		fclose(in); fclose(out);
		core->loadSave(core, VFileOpen(path, O_RDWR));
	}
	// Deterministic clock: fixed RTC (override with BOKTAI3_RTC=unix seconds)
	const char* rtc_env = getenv("BOKTAI3_RTC");
	core->rtc.override = RTC_FIXED;
	core->rtc.value = (int64_t) (rtc_env ? strtoll(rtc_env, NULL, 10) : 1128150000LL) * 1000; // 2005-10-01 07:00 UTC
	core->reset(core);

	cpu = core->cpu;
	orig = cpu->memory;
	cpu->memory.load32 = h_load32;
	cpu->memory.load16 = h_load16;
	cpu->memory.load8 = h_load8;
	cpu->memory.loadMultiple = h_loadMultiple;
#ifdef STUDIO_EXPERIMENTS
	cpu->memory.store32 = h_store32;
	cpu->memory.store16 = h_store16;
	cpu->memory.store8 = h_store8;
	cpu->memory.storeMultiple = h_storeMultiple;
	observe_attach(core, outdir);
#endif


	rom_reader = calloc(ROM_SIZE, 4);
	rom_exec = calloc(ROM_SIZE / 2, 1);
	rom_word32 = calloc(ROM_SIZE / 4, 4);
	seg_reader = NULL;
	seg_exec = NULL;

	FILE* s = fopen(argv[2], "r");
	{
		const char* sl = strrchr(argv[2], '/');
		if (sl) snprintf(scriptdir, sizeof(scriptdir), "%.*s", (int) (sl - argv[2]), argv[2]);
	}
	if (!s) { perror("script"); return 1; }
	char line[512];
	while (fgets(line, sizeof(line), s)) {
		char cmd[64] = "", a1[256] = "", a2[64] = "";
		char* hash = strchr(line, '#');
		if (hash) *hash = 0;
		if (sscanf(line, "%63s %255s %63s", cmd, a1, a2) < 1) continue;
		if (!strcmp(cmd, "wait")) { core->setKeys(core, 0); run_frames(atoi(a1)); }
		else if (!strcmp(cmd, "hold")) { core->setKeys(core, parse_keys(a1)); run_frames(atoi(a2)); core->setKeys(core, 0); }
		else if (!strcmp(cmd, "tick") || !strcmp(cmd, "fasttick")) {
			tick_pc = (uint32_t)strtoul(a1, NULL, 16);
			fast_tick_pc = 0;
			tick_hits = 0;
			tick_total = 0;
			if (cmd[0] == 'f' && !set_fast_tick(tick_pc)) {
				fprintf(stderr, "fasttick needs a Thumb literal load at entry or after PUSH\n");
				return 1;
			}
		}
		else if (!strcmp(cmd, "tickdump")) {
			// Ticks are decimal. Addresses, offsets and lengths are hex.
			char path[1024], *rest = line;
			snprintf(path, sizeof(path), "%s/%s.bin", outdir, a1);
			if (tickdump_f) fclose(tickdump_f);
			tickdump_f = fopen(path, "wb");
			if (!tickdump_f) { perror(path); return 1; }
			if (sscanf(a2, "%ld:%ld", &tickdump_from, &tickdump_to) != 2 ||
			    tickdump_from < 0 || tickdump_to <= tickdump_from) {
				fprintf(stderr, "invalid tickdump range: %s\n", a2);
				return 1;
			}
			strtok(rest, " \t\r\n");
			strtok(NULL, " \t\r\n");
			strtok(NULL, " \t\r\n");
			tickdump_n = 0;
			for (char* t = strtok(NULL, ", \t\r\n"); t; t = strtok(NULL, ", \t\r\n")) {
				unsigned addr, offset = 0, len;
				int end = 0;
				int indirect = sscanf(t, "%x+%x:%x%n", &addr, &offset, &len, &end) == 3;
				if (!indirect && sscanf(t, "%x:%x%n", &addr, &len, &end) != 2) end = 0;
				if (!end || t[end] || !len || tickdump_n == 32 ||
				    (indirect && ((addr & 3) || !tickdump_ram(addr, 4)))) {
					fprintf(stderr, "invalid tickdump region: %s\n", t);
					return 1;
				}
				tickdump_r[tickdump_n].addr = addr;
				tickdump_r[tickdump_n].offset = offset;
				tickdump_r[tickdump_n].len = len;
				tickdump_r[tickdump_n++].indirect = indirect;
			}
			if (!tickdump_n) { fprintf(stderr, "tickdump needs at least one region\n"); return 1; }
		}
		else if (!strcmp(cmd, "holdt")) {
			int want = atoi(a2), f = 0;
			core->setKeys(core, strcmp(a1, "NONE") ? parse_keys(a1) : 0);
			tick_hits = 0;
			while (tick_pc && tick_hits < want && f < want * 60 + 600) { run_frames(1); ++f; }
			if (tick_hits < want) fprintf(stderr, "holdt: %d of %d ticks\n", tick_hits, want);
		}
		else if (!strcmp(cmd, "tap")) { core->setKeys(core, parse_keys(a1)); run_frames(2); core->setKeys(core, 0); run_frames(8); }
		else if (!strcmp(cmd, "shot")) shot(a1);
		else if (!strcmp(cmd, "save")) savestate(a1, 0);
		else if (!strcmp(cmd, "load")) savestate(a1, 1);
		else if (!strcmp(cmd, "loadstate")) loadstate(a1);
#ifdef USE_FFMPEG
		else if (!strcmp(cmd, "record")) {
			FFmpegEncoderInit(&encoder);
			if (!FFmpegEncoderSetAudio(&encoder, "aac", 192 * 1024) || !FFmpegEncoderSetVideo(&encoder, "libx264", -18, 0)
			    || !FFmpegEncoderSetContainer(&encoder, "mp4")) { fprintf(stderr, "encoder setup failed\n"); exit(1); }
			FFmpegEncoderSetDimensions(&encoder, W * 4, H * 4);
			if (!FFmpegEncoderOpen(&encoder, a1)) { fprintf(stderr, "cannot record to %s\n", a1); exit(1); }
			core->setAVStream(core, &encoder.d);
			recording = 1;
		}
		else if (!strcmp(cmd, "stoprecord")) stop_recording();
#endif
		else if (!strcmp(cmd, "dump")) {
			// dump NAME ADDR:LEN  (hex) -> OUTDIR/NAME.bin
			unsigned addr = 0, len = 0;
			sscanf(a2, "%x:%x", &addr, &len);
			char path[1024];
			snprintf(path, sizeof(path), "%s/%s.bin", outdir, a1);
			FILE* f = fopen(path, "wb");
			for (unsigned i = 0; i < len; ++i) fputc(core->rawRead8(core, addr + i, -1), f);
			fclose(f);
		}
		else if (!strcmp(cmd, "watch")) {
			unsigned addr = 0, len = 4;
			sscanf(a1, "%x:%x", &addr, &len);
			watch_lo = addr; watch_hi = addr + len;
			cpu->memory.store32 = h_store32;
			cpu->memory.store16 = h_store16;
			cpu->memory.store8 = h_store8;
			cpu->memory.storeMultiple = h_storeMultiple;
		}
		else if (!strcmp(cmd, "poke") || !strcmp(cmd, "freeze")) {
			// poke ADDR[:SIZE] VALUE   (hex; SIZE 1/2/4, default 4)
			// freeze ADDR[:SIZE] VALUE  same, rewritten before every frame
			unsigned addr = 0, size = 4, val = 0;
			sscanf(a1, "%x:%x", &addr, &size);
			sscanf(a2, "%x", &val);
			poke(addr, size, val);
			if (cmd[0] == 'f' && nfreeze < 32) {
				freezes[nfreeze].addr = addr; freezes[nfreeze].size = size; freezes[nfreeze].val = val;
				nfreeze++;
			}
		}
		else if (!strcmp(cmd, "unfreeze")) nfreeze = 0;
		else if (!strcmp(cmd, "lux")) lux = atoi(a1);
		else if (!strcmp(cmd, "rtc")) core->rtc.value = (int64_t) strtoll(a1, NULL, 10) * 1000;
		else if (!strcmp(cmd, "trace")) tracing = !strcmp(a1, "on");
		else if (!strcmp(cmd, "probe")) add_probe(a1, a2);
		else if (!strcmp(cmd, "watchpc")) {
			unsigned addr = 0; int lo = 0, hi = 0;
			if (sscanf(line, "%*s %x %d %d", &addr, &lo, &hi) != 3) { fprintf(stderr, "watchpc ADDR LO HI\n"); return 1; }
			until_pc = addr; until_lo = lo; until_hi = hi; until_hit = 0;
		}
		else if (!strcmp(cmd, "waitpc")) {
			int f = 0, max = atoi(a1), want = *a2 ? atoi(a2) : 1;
			while (until_pc && until_hit < want && f < max) { run_frames(1); ++f; }
			if (until_hit < want) fprintf(stderr, "waitpc %X: reached %d of %d times in %d frames\n", until_pc, until_hit, want, max);
			until_pc = 0;
		}
		else if (!strcmp(cmd, "unprobe")) { nprobes = 0; probe_lo = UINT32_MAX; probe_hi = 0; }
		else if (!strcmp(cmd, "mark")) {
			end_segment();
			if (!seg_reader) { seg_reader = calloc(ROM_SIZE, 4); seg_exec = calloc(ROM_SIZE / 2, 1); }
			memset(seg_reader, 0, ROM_SIZE * 4);
			memset(seg_exec, 0, ROM_SIZE / 2);
			strncpy(seg_name, a1, sizeof(seg_name) - 1);
		}
		else if (!observe_command(line) && !draw_trace_command(core, outdir, line)) { fprintf(stderr, "unknown command %s\n", cmd); return 1; }
		fprintf(stderr, "[frame %u] %s %s %s\n", core->frameCounter(core), cmd, a1, a2);
	}
	end_segment();
	observe_close();
	draw_trace_close();
#ifdef USE_FFMPEG
	stop_recording();
#endif
	if (probe_file && fclose(probe_file)) { perror("probes.tsv"); return 1; }
	if (tickdump_f && fclose(tickdump_f)) { perror("tickdump"); return 1; }
	if (getenv("BOKTAI3_NO_COVERAGE")) {
		core->deinit(core);
		return 0;
	}
#ifdef STUDIO_EXPERIMENTS
	if (getenv("BOKTAI3_OBSERVATIONS_ONLY")) {
		core->deinit(core);
		return 0;
	}
#endif
	write_cov("total", rom_reader, rom_exec);
	char path[1024];
	snprintf(path, sizeof(path), "%s/total.word32", outdir);
	FILE* fw = fopen(path, "wb"); fwrite(rom_word32, 4, ROM_SIZE / 4, fw); fclose(fw);
	snprintf(path, sizeof(path), "%s/total.iwram_exec", outdir);
	FILE* f = fopen(path, "wb"); fwrite(iwram_exec, 1, sizeof(iwram_exec), f); fclose(f);
	snprintf(path, sizeof(path), "%s/total.ewram_exec", outdir);
	f = fopen(path, "wb"); fwrite(ewram_exec, 1, sizeof(ewram_exec), f); fclose(f);
	core->deinit(core);
	return 0;
}
