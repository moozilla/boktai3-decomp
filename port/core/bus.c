#include "platform.h"
#include "hw.h"
#include "devices.h"

/* Full-game builds enable spin detection: a loop re-reading one volatile
 * address with no store in between is waiting for time to pass. */
static int spin_enabled;
static uintptr_t spin_address;
static unsigned spin_count;
void port_bus_enable_spin(int enabled) { spin_enabled = enabled; spin_count = 0; }
void port_watchdog(const char *what, uint32_t value);
static unsigned long watchdog;
void port_bus_frame_elapsed(void) { watchdog = 0; }
static void spin(uintptr_t address)
{
    if (!spin_enabled) return;
    if (++watchdog == 50000000ul) port_watchdog("bus: no frame after 50M volatile loads; last", (uint32_t)address);
    if (address != spin_address) {
        spin_address = address;
        spin_count = 0;
    } else if (++spin_count >= 3) {
        spin_count = 0;
        port_spin_advance();
    }
}

static int is_device(uintptr_t address)
{
    return (address >= 0x04000000 && address < 0x04000400) ||
           (address >= 0x080000c4 && address < 0x080000ca) ||
           (address >= 0x0d000000 && address < 0x0f000000);
}

uint8_t port_bus_load_i8(const volatile void *p)
{
    Hw *hw = hw_bound();
    uintptr_t address = (uintptr_t)p;
    spin(address);
    if (hw && is_device(address)) {
        port_devices_sync_timing();
        return hw_read8(hw, (uint32_t)address);
    }
    return *(const volatile uint8_t *)p;
}
uint16_t port_bus_load_i16(const volatile void *p)
{
    Hw *hw = hw_bound();
    uintptr_t address = (uintptr_t)p;
    spin(address);
    if (hw && is_device(address)) {
        port_devices_sync_timing();
        return hw_read16(hw, (uint32_t)address);
    }
    return *(const volatile uint16_t *)p;
}
uint32_t port_bus_load_i32(const volatile void *p)
{
    Hw *hw = hw_bound();
    uintptr_t address = (uintptr_t)p;
    spin(address);
    if (hw && is_device(address)) {
        port_devices_sync_timing();
        return hw_read32(hw, (uint32_t)address);
    }
    return *(const volatile uint32_t *)p;
}
void *port_bus_load_ptr(const volatile void *p)
{
    return (void *)(uintptr_t)port_bus_load_i32(p);
}
void port_bus_store_i8(volatile void *p, uint8_t value)
{
    Hw *hw = hw_bound();
    uintptr_t address = (uintptr_t)p;
    spin_address = 0;
    if (hw && is_device(address)) {
        port_devices_sync_timing();
        hw_write8(hw, (uint32_t)address, value);
    }
    else *(volatile uint8_t *)p = value;
    port_devices_notify_write((uint32_t)address, 1);
}
void port_bus_store_i16(volatile void *p, uint16_t value)
{
    Hw *hw = hw_bound();
    uintptr_t address = (uintptr_t)p;
    spin_address = 0;
    if (hw && is_device(address)) {
        port_devices_sync_timing();
        hw_write16(hw, (uint32_t)address, value);
    }
    else *(volatile uint16_t *)p = value;
    port_devices_notify_write((uint32_t)address, 2);
}
void port_bus_store_i32(volatile void *p, uint32_t value)
{
    Hw *hw = hw_bound();
    uintptr_t address = (uintptr_t)p;
    spin_address = 0;
    if (hw && is_device(address)) {
        port_devices_sync_timing();
        hw_write32(hw, (uint32_t)address, value);
    }
    else *(volatile uint32_t *)p = value;
    port_devices_notify_write((uint32_t)address, 4);
}
void port_bus_store_ptr(volatile void *p, const void *value)
{
    port_bus_store_i32(p, (uint32_t)(uintptr_t)value);
}
