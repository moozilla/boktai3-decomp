/* Function entry/exit hooks for the agent harness (port/agent/).
 *
 * port/build_game.py calls port_hook_enter(address, r0..r3) at the start of
 * every game function that has a GBA address and port_hook_exit(address, r0)
 * before each return. With nothing armed both return at the first test.
 *
 * The host arms them through the state block (port_hook_state) and the flag
 * table (port_hook_flags, one byte per ROM halfword):
 *   tracing: append an event per entry/exit to a ring in free GBA address
 *            space (0x02100000; the port maps nothing between EWRAM's end and
 *            IWRAM), optionally only inside one function's calls (root);
 *   flags:   BREAK_ENTER / BREAK_EXIT call the host's hook import, which can
 *            stop there and serve commands (read memory, call functions, run
 *            C snippets) before the game continues.
 * suspend is set by the host while it runs its own calls into the game. */
#include <stdint.h>
#include "host.h"
#include "platform.h"

#define BREAK_ENTER 1u
#define BREAK_EXIT 2u
#define ROM_CODE_BASE 0x08000000u
#define ROM_CODE_SPAN 0x00400000u
#define IWRAM_CODE 0x03000000u
#define IWRAM_CODE_ROM 0x08000350u
#define IWRAM_CODE_SIZE 0x110cu
#define RING_BASE 0x02100000u
#define RING_BYTES 0x00700000u
#define EVENT_WORDS 6u /* kind|address, frame, r0..r3 (exit: r0 = result) */
#define EVENT_EXIT 1u  /* low bit of word 0: addresses are halfword aligned */
#define EVENT_FRAME 0xfffffffeu

struct HookState {
    uint32_t active;   /* tracing || armed breakpoints: the fast-path test */
    uint32_t tracing;
    uint32_t suspend;
    uint32_t root;     /* trace only inside this function (0 = everything) */
    uint32_t inside;
    uint32_t count;    /* events in the ring */
    uint32_t capacity;
    uint32_t overflow; /* events dropped because the ring was full */
    uint32_t hit_kind; /* last stop: 1 entry, 2 exit */
    uint32_t hit_address;
    uint32_t args[4];  /* entry: r0..r3; exit: result in args[0] */
    uint32_t depth;
    uint32_t frame;
};

static struct HookState state = {0, 0, 0, 0, 0, 0, RING_BYTES / (4 * EVENT_WORDS)};
static uint8_t flags[ROM_CODE_SPAN / 2];

PORT_IMPORT(hook) void port_host_hook(uint32_t kind, uint32_t address);

struct HookState *port_hook_state(void) { return &state; }
uint8_t *port_hook_flags(void) { return flags; }
uint32_t port_hook_ring(void) { return RING_BASE; }

static uint32_t index_of(uint32_t address)
{
    if (address - IWRAM_CODE < IWRAM_CODE_SIZE)
        address = address - IWRAM_CODE + IWRAM_CODE_ROM;
    return (address - ROM_CODE_BASE) >> 1;
}

static void record(uint32_t word0, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    uint32_t *event;
    if (state.count >= state.capacity) {
        state.overflow++;
        return;
    }
    event = (uint32_t *)(uintptr_t)(RING_BASE + state.count * 4 * EVENT_WORDS);
    event[0] = word0;
    event[1] = state.frame;
    event[2] = a0;
    event[3] = a1;
    event[4] = a2;
    event[5] = a3;
    state.count++;
}

void port_hook_frame(uint32_t frame)
{
    state.frame = frame;
    if (state.tracing && !state.suspend && !state.root)
        record(EVENT_FRAME, frame, 0, 0, 0);
}

void port_hook_enter(uint32_t address, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    uint32_t i;
    if (!state.active || state.suspend)
        return;
    state.depth++;
    if (state.tracing) {
        if (address == state.root)
            state.inside++;
        if (!state.root || state.inside)
            record(address & ~1u, a0, a1, a2, a3);
    }
    i = index_of(address);
    if (i < sizeof(flags) && (flags[i] & BREAK_ENTER)) {
        state.hit_kind = 1;
        state.hit_address = address & ~1u;
        state.args[0] = a0;
        state.args[1] = a1;
        state.args[2] = a2;
        state.args[3] = a3;
        port_host_hook(1, address & ~1u);
    }
}

void port_hook_exit(uint32_t address, uint32_t result)
{
    uint32_t i;
    if (!state.active || state.suspend)
        return;
    if (state.depth)
        state.depth--;
    if (state.tracing) {
        if (!state.root || state.inside)
            record((address & ~1u) | EVENT_EXIT, result, 0, 0, 0);
        if (address == state.root && state.inside)
            state.inside--;
    }
    i = index_of(address);
    if (i < sizeof(flags) && (flags[i] & BREAK_EXIT)) {
        state.hit_kind = 2;
        state.hit_address = address & ~1u;
        state.args[0] = result;
        port_host_hook(2, address & ~1u);
    }
}
