#include "platform.h"
#if !defined(__wasm__)
#include <sys/mman.h>
#include <unistd.h>
#endif

struct Region { uint32_t address, size; };
static const struct Region regions[] = {
    {PORT_EWRAM_BASE, PORT_EWRAM_SIZE}, {PORT_IWRAM_BASE, PORT_IWRAM_SIZE},
    {PORT_IO_BASE, PORT_IO_SIZE}, {PORT_PALETTE_BASE, PORT_PALETTE_SIZE},
    {PORT_VRAM_BASE, PORT_VRAM_SIZE}, {PORT_OAM_BASE, PORT_OAM_SIZE},
    {PORT_ROM_BASE, PORT_ROM_CAPACITY}
};
static int initialized;
static size_t rom_length;
#if !defined(__wasm__)
static size_t mapped_sizes[sizeof(regions) / sizeof(regions[0])];
#endif

void *port_memory_pointer(uint32_t address, size_t length)
{
    unsigned i;
    if (!initialized) return NULL;
    for (i = 0; i < sizeof(regions) / sizeof(regions[0]); ++i) {
        if (address >= regions[i].address &&
            address - regions[i].address <= regions[i].size &&
            length <= regions[i].size - (address - regions[i].address))
            return (void *)(uintptr_t)address;
    }
    return NULL;
}

int port_memory_init(void)
{
    if (initialized) return 0;
#if defined(__wasm__)
    if ((uint64_t)__builtin_wasm_memory_size(0) * 65536 < PORT_MEMORY_END)
        return -1;
#else
    unsigned i;
    size_t page = (size_t)sysconf(_SC_PAGESIZE);
    for (i = 0; i < sizeof(regions) / sizeof(regions[0]); ++i) {
        size_t length = (regions[i].size + page - 1) & ~(page - 1);
        void *wanted = (void *)(uintptr_t)regions[i].address;
        /* A hint + exact-address check avoids MAP_FIXED clobbering the host. */
        void *p = mmap(wanted, length, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANON, -1, 0);
        if (p == MAP_FAILED || p != wanted) {
            if (p != MAP_FAILED) munmap(p, length);
            while (i) { --i; munmap((void *)(uintptr_t)regions[i].address, mapped_sizes[i]); }
            return -1;
        }
        mapped_sizes[i] = length;
    }
#endif
    initialized = 1;
    rom_length = 0;
    port_memory_reset();
    return 0;
}

void port_memory_destroy(void)
{
#if !defined(__wasm__)
    unsigned i;
    if (initialized)
        for (i = 0; i < sizeof(regions) / sizeof(regions[0]); ++i)
            munmap((void *)(uintptr_t)regions[i].address, mapped_sizes[i]);
#endif
    initialized = 0;
    rom_length = 0;
}

void port_memory_reset(void)
{
    unsigned i;
    if (!initialized) return;
    for (i = 0; i + 1 < sizeof(regions) / sizeof(regions[0]); ++i) {
        uint8_t *p = (uint8_t *)(uintptr_t)regions[i].address;
        size_t n;
        for (n = 0; n < regions[i].size; ++n) p[n] = 0;
    }
    *(uint16_t *)(uintptr_t)(PORT_IO_BASE + 0x130) = 0x03ff;
}

int port_load_rom(const void *bytes, size_t length)
{
    size_t i;
    uint8_t *dest;
    const uint8_t *src = bytes;
    if (!bytes || length < 0xc0 || length > PORT_ROM_CAPACITY || !initialized) return -1;
    dest = (uint8_t *)(uintptr_t)PORT_ROM_BASE;
    /* memmove semantics allow a browser to stage bytes in the ROM region. */
    if ((uintptr_t)dest > (uintptr_t)src && (uintptr_t)dest - (uintptr_t)src < length)
        for (i = length; i; --i) dest[i - 1] = src[i - 1];
    else
        for (i = 0; i < length; ++i) dest[i] = src[i];
    for (i = length; i < rom_length; ++i) dest[i] = 0;
    rom_length = length;
    return 0;
}

size_t port_rom_size(void) { return rom_length; }
