#include "platform.h"
#include "boot.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef PORT_TEST_RTC
#include "devices.h"
void Clock_CheckRtcDate(void);
#endif
int main(int argc, char **argv)
{
    FILE *file;
    long length;
    unsigned char *rom;
    unsigned i;
    assert(argc == 3);
    file = fopen(argv[1], "rb");
    assert(file);
    assert(fseek(file, 0, SEEK_END) == 0);
    length = ftell(file);
    assert(length > 0 && (unsigned long)length <= PORT_ROM_CAPACITY);
    rewind(file);
    rom = malloc((size_t)length);
    assert(rom && fread(rom, 1, (size_t)length, file) == (size_t)length);
    fclose(file);
    assert(port_memory_init() == 0);
    assert(port_load_rom(rom, (size_t)length) == 0);
    free(rom);
    memset((void *)(uintptr_t)PORT_IWRAM_BASE, 0xa5, PORT_IWRAM_SIZE);
    *(uint32_t *)(uintptr_t)0x0201f400 = 0x12345678;
    *(uint32_t *)(uintptr_t)0x0201f404 = 0x87654321;
    assert(port_boot_early() == 0 && port_boot_call_count() == 2);
    assert(*(uint32_t *)(uintptr_t)0x0201f400 == 0x12345678);
    assert(*(uint32_t *)(uintptr_t)0x0201f404 == 0x87654321);
    assert(*(uint32_t *)(uintptr_t)0x03007d00 == 0xa5a5a5a5);
    assert(*(uint16_t *)(uintptr_t)0x04000204 == 0x4014);
    assert(*(uint16_t *)(uintptr_t)0x080000c8 == 1);
    assert(*(uint8_t *)(uintptr_t)0x030035c6 == 0);
    for (i = 0; i < 0x24; ++i) assert(*(uint8_t *)(uintptr_t)(0x03005430 + i) == 0);
#ifdef PORT_TEST_RTC
    port_devices_init(NULL, NULL);
    Clock_CheckRtcDate();
#endif
    file = fopen(argv[2], "wb");
    assert(file && fwrite((void *)(uintptr_t)0x03005430, 1, 0x24, file) == 0x24);
#ifdef PORT_TEST_RTC
    assert(fwrite((void *)(uintptr_t)0x03005460, 1, 8, file) == 8);
    puts("RTC: original Clock_CheckRtcDate and GPIO protocol executed");
#endif
    fclose(file);
    assert(port_boot_copy_iwram() == 0);
    assert(memcmp((void *)(uintptr_t)0x03000000, (void *)(uintptr_t)0x08000350, 0x110c) == 0);
    puts("boot: game C Rtc_Initialize -> Rtc_EnableGpioReads -> Clock_ClearState -> Memory_Clear -> CpuSet PASS");
    port_memory_destroy();
    return 0;
}
