/* Code pointers in the full game are GBA addresses (ROM tables and C agree).
 * The ABI pass (port/core/abi.py) routes every indirect call here; the table
 * of canonical adapters is generated at link time from the ROM ELF symbols. */
#include <stdint.h>
#include "host.h"
#include "platform.h"

typedef int64_t (*PortThunk)(uint32_t, uint32_t, uint32_t, uint32_t,
                             uint32_t, uint32_t, uint32_t, uint32_t,
                             uint32_t, uint32_t, uint32_t, uint32_t);
struct PortCode { uint32_t address; PortThunk thunk; };
extern const struct PortCode port_code_table[];
extern const uint32_t port_code_count;

/* AgbMain copies ROM 08000350..0800145C (ARM C) to IWRAM 03000000. */
#define IWRAM_CODE 0x03000000u
#define IWRAM_CODE_ROM 0x08000350u
#define IWRAM_CODE_SIZE 0x110cu

/* ARM r0..r3 at the most recent call into a register-reading function. */
uint32_t port_regs[4];

static PortThunk lookup(uint32_t address)
{
    uint32_t lo = 0, hi = port_code_count;
    address &= ~1u;
    if (address - IWRAM_CODE < IWRAM_CODE_SIZE)
        address = address - IWRAM_CODE + IWRAM_CODE_ROM;
    while (lo < hi) {
        uint32_t mid = (lo + hi) / 2;
        uint32_t key = port_code_table[mid].address;
        if (key == address) return port_code_table[mid].thunk;
        if (key < address) lo = mid + 1;
        else hi = mid;
    }
    return 0;
}

int port_code_known(uint32_t address) { return lookup(address) != 0; }

int64_t port_icall(void *function, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3,
                   uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7,
                   uint32_t a8, uint32_t a9, uint32_t a10, uint32_t a11)
{
    PortThunk thunk = lookup((uint32_t)(uintptr_t)function);
    if (!thunk) {
        port_host_fatal("call to unknown code address", (uint32_t)(uintptr_t)function);
        __builtin_trap();
    }
    return thunk(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
}

int port_code_invoke(uint32_t address)
{
    PortThunk thunk = lookup(address);
    if (!thunk) return -1;
    thunk(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return 0;
}

/* --trace builds: one byte per ROM halfword, set when a function is entered
 * (the layout of the mGBA harness's .exec coverage files). */
static uint8_t trace_map[0x800000];
void port_trace(uint32_t address)
{
    uint32_t i = (address - 0x08000000u) >> 1;
    if (i < sizeof(trace_map)) trace_map[i] = 1;
}
uint8_t *port_trace_map(void) { return trace_map; }

/* --watch builds: log a function's address and first three arguments. */
void port_watch(uint32_t address, uint32_t a0, uint32_t a1, uint32_t a2)
{
    static const char hex[] = "0123456789abcdef";
    uint32_t words[4] = {address, a0, a1, a2};
    char text[48], *p = text;
    int i, k;
    for (i = 0; i < 4; i++) {
        *p++ = i ? ' ' : 'w';
        for (k = 28; k >= 0; k -= 4) *p++ = hex[(words[i] >> k) & 15];
    }
    *p = 0;
    port_host_log(text, (uint32_t)port_frame_count());
}
