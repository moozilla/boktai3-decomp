/* Flight recorder runtime, linked into the --record build of the port
 * (port/build_game.py --record; tools/recorder/instrument.py inserts the calls).
 * Never instrumented itself.
 *
 * Events are u32 words in a ring at RING_BASE (free GBA address space). The
 * first word is the site id: kind (3 bits) | unit (13) | local (16). By kind:
 *   0 BB    [site]                                   basic block entered
 *   1 LOAD  [site, address | wcode << 28, value]      wcode: 0 byte, 1 half, 2 word
 *   2 STORE [site, address | wcode << 28, value]
 *   3 WIDE  [site, address | 3 << 28 | store << 31, low, high]
 *   4 CPY   [site, dst, src, length, data words...]   the source bytes, before the copy
 *   5 SET   [site, dst, byte, length]
 *   6 AGG   [site, address, size | store << 31, data words...] memory after the access
 *   7 FN    [site, 0 entry | 1 return]
 * Modes: REC logs stores, copies and fills that touch GBA RAM, IO or the module's
 * static data (the write log);
 * TRACE logs every event (re-execution for slicing). The host drains the ring at
 * every frame and when it fills (the rec_flush import). */
#include <stdint.h>

#define RING_BASE 0x04100000u
#define RING_WORDS ((0x05000000u - RING_BASE) / 4u)
#define MODE_REC 1u
#define MODE_TRACE 2u

struct RecState {
    uint32_t mode;
    uint32_t count;    /* words in the ring */
    uint32_t capacity; /* words */
    uint32_t overflow; /* flushes requested by a full ring */
    uint32_t ring;
    uint32_t lost;     /* events dropped because the host did not drain */
    /* perturbation (in-frame validation): right after the Nth execution of the store at
     * SITE (counted from when the host armed it), xor the stored bytes with XOR */
    uint32_t perturb_site;
    uint32_t perturb_left;
    uint32_t perturb_xor_lo;
    uint32_t perturb_xor_hi;
};

static struct RecState state = {0, 0, RING_WORDS, 0, RING_BASE, 0, 0, 0, 0, 0};

static void perturb(uint32_t site, void *address, uint32_t bytes)
{
    uint8_t *p = (uint8_t *)address;
    uint32_t i;
    if (site != state.perturb_site || !state.perturb_left || --state.perturb_left) return;
    for (i = 0; i < bytes && i < 8; i++)
        p[i] ^= (uint8_t)((i < 4 ? state.perturb_xor_lo >> (8 * i) : state.perturb_xor_hi >> (8 * (i - 4))) & 0xFF);
}

__attribute__((import_module("env"), import_name("rec_flush"))) void rec_host_flush(void);

struct RecState *rec_state(void) { return &state; }

/* The module's static data (port runtime state: devices, timing, game.c's solar
 * level): logged too, so a slice can cross frames through it. The stack is not. */
extern unsigned char __global_base, __heap_base;

/* Game-side units (bit per unit index; the host sets it: everything but the
 * emulated hardware). Only their writes to IO and to the module's static data are
 * logged: the hardware's own (HBlank DMA, DISPSTAT, audio and timing state) follow
 * from logged RAM and would dominate the log. RAM writes are always logged. */
static uint8_t static_units[8192 / 8];

uint8_t *rec_static_units(void) { return static_units; }

static int game_unit(uint32_t site)
{
    uint32_t unit = (site >> 16) & 0x1FFFu;
    return static_units[unit >> 3] >> (unit & 7) & 1;
}

/* Trace mode logs everything game-side; from the hardware units only what record
 * mode logs (their RAM writes), so DMA still shows up as data flow. */
static int traced(uint32_t site) { return (state.mode & MODE_TRACE) && game_unit(site); }

static int watched_site(uint32_t site, uint32_t address, uint32_t length)
{
    uint32_t end = address + (length ? length : 1), unit = (site >> 16) & 0x1FFFu;
    if (address < (uint32_t)(uintptr_t)&__heap_base && end > (uint32_t)(uintptr_t)&__global_base)
        return static_units[unit >> 3] >> (unit & 7) & 1;
    if (address < 0x02040000u && end > 0x02000000u) return 1;
    if (address < 0x03008000u && end > 0x03000000u) return 1;
    if (address < 0x04000400u && end > 0x04000000u) return static_units[unit >> 3] >> (unit & 7) & 1;
    return 0;
}

static uint32_t *reserve(uint32_t words)
{
    uint32_t *at;
    if (state.count + words > state.capacity) {
        state.overflow++;
        rec_host_flush();
        if (state.count + words > state.capacity) {
            state.lost++;
            return 0;
        }
    }
    at = (uint32_t *)(uintptr_t)(state.ring + 4u * state.count);
    state.count += words;
    return at;
}

void __rec_bb(uint32_t site)
{
    uint32_t *e;
    if (!traced(site)) return;
    if ((e = reserve(1))) e[0] = site;
}

void __rec_fn(uint32_t site, uint32_t exit)
{
    uint32_t *e;
    if (!traced(site)) return;
    if ((e = reserve(2))) { e[0] = site; e[1] = exit; }
}

void __rec_ld(uint32_t site, const void *address, uint32_t value, uint32_t wcode)
{
    uint32_t *e;
    if (!traced(site)) return;
    if ((e = reserve(3))) { e[0] = site; e[1] = (uint32_t)(uintptr_t)address | wcode << 28; e[2] = value; }
}

void __rec_st(uint32_t site, void *address, uint32_t value, uint32_t wcode)
{
    uint32_t *e;
    if (state.perturb_left) perturb(site, address, 1u << wcode);
    if (!state.mode) return;
    if (!traced(site) && !watched_site(site, (uint32_t)(uintptr_t)address, 1u << wcode)) return;
    if ((e = reserve(3))) { e[0] = site; e[1] = (uint32_t)(uintptr_t)address | wcode << 28; e[2] = value; }
}

void __rec_ld64(uint32_t site, const void *address, uint64_t value)
{
    uint32_t *e;
    if (!traced(site)) return;
    if ((e = reserve(4))) { e[0] = site; e[1] = (uint32_t)(uintptr_t)address | 3u << 28; e[2] = (uint32_t)value; e[3] = (uint32_t)(value >> 32); }
}

void __rec_st64(uint32_t site, void *address, uint64_t value)
{
    uint32_t *e;
    if (state.perturb_left) perturb(site, address, 8);
    if (!state.mode) return;
    if (!traced(site) && !watched_site(site, (uint32_t)(uintptr_t)address, 8)) return;
    if ((e = reserve(4))) {
        e[0] = site; e[1] = (uint32_t)(uintptr_t)address | 3u << 28 | 1u << 31;
        e[2] = (uint32_t)value; e[3] = (uint32_t)(value >> 32);
    }
}

static void copy_words(uint32_t *to, const uint8_t *from, uint32_t length)
{
    uint32_t i;
    for (i = 0; i < (length + 3u) / 4u; i++) to[i] = 0;
    for (i = 0; i < length; i++) to[i / 4u] |= (uint32_t)from[i] << (8u * (i & 3u));
}

void __rec_cpy(uint32_t site, void *dst, const void *src, uint32_t length)
{
    uint32_t *e;
    if (!state.mode) return;
    if (!traced(site) && !watched_site(site, (uint32_t)(uintptr_t)dst, length)) return;
    if (length > 0x100000u) length = 0x100000u;
    if ((e = reserve(4u + (length + 3u) / 4u))) {
        e[0] = site; e[1] = (uint32_t)(uintptr_t)dst; e[2] = (uint32_t)(uintptr_t)src; e[3] = length;
        copy_words(e + 4, (const uint8_t *)src, length);
    }
}

void __rec_set(uint32_t site, void *dst, uint32_t byte, uint32_t length)
{
    uint32_t *e;
    if (!state.mode) return;
    if (!traced(site) && !watched_site(site, (uint32_t)(uintptr_t)dst, length)) return;
    if ((e = reserve(4))) { e[0] = site; e[1] = (uint32_t)(uintptr_t)dst; e[2] = byte & 0xffu; e[3] = length; }
}

/* An aggregate load or store: the instrumentation calls these after the access,
 * so for a store the bytes in memory are the stored value. */
static void aggregate(uint32_t site, void *address, uint32_t size, uint32_t store)
{
    uint32_t *e;
    if (size > 0x10000u) size = 0x10000u;
    if ((e = reserve(3u + (size + 3u) / 4u))) {
        e[0] = site; e[1] = (uint32_t)(uintptr_t)address; e[2] = size | store << 31;
        copy_words(e + 3, (const uint8_t *)address, size);
    }
}

void __rec_agg(uint32_t site, void *address, uint32_t size)
{
    if (traced(site)) aggregate(site, address, size, 0);
}

void __rec_aggst(uint32_t site, void *address, uint32_t size)
{
    if (state.perturb_left) perturb(site, address, size);
    if (!state.mode) return;
    if (!traced(site) && !watched_site(site, (uint32_t)(uintptr_t)address, size)) return;
    aggregate(site, address, size, 1);
}
