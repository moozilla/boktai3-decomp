/* Full-game entry for the source port: runs the original AgbMain. Time passes
 * only at wait points (BIOS waits, spin loops), where core timing draws
 * scanlines, raises interrupts and presents each frame to the host. */
#include "platform.h"
#include "devices.h"
#include "hw.h"
#include "host.h"

void AgbMain(void);
int port_video_init(void);
const uint16_t *port_video_framebuffer(void);
int port_code_invoke(uint32_t address);
void port_bus_enable_spin(int enabled);

static uint32_t frames;
static uint32_t solar_level = 232; /* the hardware model's default: no sunlight */
static uint8_t save_stage[HW_SAVE_MAX];

static int invoke(void *context, uint32_t address)
{
    (void)context;
    if (port_code_invoke(address) < 0) {
        port_host_fatal("interrupt handler at unknown address", address);
        __builtin_trap();
    }
    return 0;
}

void port_bus_frame_elapsed(void);
void port_watchdog(const char *what, uint32_t value) { port_host_log(what, value); }

static void frame(void)
{
    Hw *hw = port_devices_hardware();
    port_bus_frame_elapsed();
    /* SoundMain leaves one VBlank of mixed PCM in the hardware model. */
    if (hw->audio_frames) {
        port_host_audio(hw->audio_buffer, (uint32_t)hw->audio_frames, hw->audio_rate);
        hw->audio_frames = 0;
    }
    uint32_t keys = port_host_frame(port_video_framebuffer(), frames++);
    hw_set_keys(port_devices_hardware(), (uint16_t)(keys & 0x3ff));
}


void port_system_call(unsigned number) { hw_system_call(number); }

/* agbcc's soft-float doubles keep the high word first (the FPA layout). The
 * newlib tables the game reads from ROM are converted once after loading.
 * port/build_game.py checks this list against every extern double in src/. */
static const struct { uint32_t address, count; } rom_doubles[] = {
    {0x08602D98, 4},  /* atanhi */
    {0x08602DB8, 4},  /* atanlo */
    {0x08602DD8, 11}, /* aT */
    {0x08602F48, 2},  /* fmod Zero */
    {0x08603188, 8},  /* PIo2 */
    {0x08603238, 13}, /* tan T */
};

static void swap_rom_doubles(void)
{
    unsigned i, j;
    for (i = 0; i < sizeof(rom_doubles) / sizeof(rom_doubles[0]); i++) {
        uint32_t *word = (uint32_t *)(uintptr_t)rom_doubles[i].address;
        for (j = 0; j < rom_doubles[i].count; j++, word += 2) {
            uint32_t high = word[0];
            word[0] = word[1];
            word[1] = high;
        }
    }
}

/* 0 = bright sunlight .. 255 = dark (the cartridge's light sensor). */
void port_game_set_solar(uint32_t level)
{
    solar_level = level > 255 ? 255 : level;
    if (port_devices_hardware()) hw_set_solar(port_devices_hardware(), solar_level);
}

/* RTC wall clock in seconds since 1970 (the host passes local time). */
void port_game_set_time(uint32_t unix_seconds) { port_devices_set_time((int64_t)unix_seconds); }

/* Battery save (EEPROM, 8 KiB, emulator .sav byte order). The host writes a
 * save into the stage buffer before port_game_run and polls for changes. */
uint8_t *port_game_save_stage(void) { return save_stage; }
int port_game_save_dirty(void)
{
    Hw *hw = port_devices_hardware();
    return hw && hw->save_dirty;
}
const uint8_t *port_game_save_data(void)
{
    Hw *hw = port_devices_hardware();
    if (!hw) return 0;
    hw_save_clean(hw);
    return hw->save;
}
uint32_t port_game_save_size(void) { return 8192; }

int port_game_run(uint32_t save_size)
{
    if (port_rom_size() < 0x200000) return -1;
    swap_rom_doubles();
    port_devices_init(NULL, NULL);
    port_devices_set_game(invoke, frame, NULL);
    hw_set_solar(port_devices_hardware(), solar_level);
    /* AgbMain's Eeprom_SetCapacity(64) keeps a save already configured. */
    hw_save_configure(port_devices_hardware(), HW_SAVE_EEPROM_8K);
    if (save_size == 8192) hw_save_load(port_devices_hardware(), save_stage, save_size);
    if (port_video_init() < 0) return -2;
    port_bus_enable_spin(1);
    AgbMain();
    return 0;
}
