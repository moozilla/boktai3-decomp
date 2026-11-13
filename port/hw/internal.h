#ifndef BOKTAI_PORT_HW_INTERNAL_H
#define BOKTAI_PORT_HW_INTERNAL_H
#include "hw.h"

static inline uint16_t hw_get16(const uint8_t *p)
{
    return (uint16_t)(p[0] | (uint16_t)p[1] << 8);
}
static inline uint32_t hw_get32(const uint8_t *p)
{
    return (uint32_t)hw_get16(p) | (uint32_t)hw_get16(p + 2) << 16;
}
static inline void hw_put16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8);
}
static inline void hw_put32(uint8_t *p, uint32_t v)
{
    hw_put16(p, (uint16_t)v); hw_put16(p + 2, (uint16_t)(v >> 16));
}
static inline void hw_zero(void *ptr, size_t n)
{
    uint8_t *p = ptr;
    while (n--) *p++ = 0;
}
static inline void hw_copy(void *dest, const void *source, size_t n)
{
    uint8_t *d = dest;
    const uint8_t *s = source;
    while (n--) *d++ = *s++;
}
uint8_t *hw_map(Hw *hw, uint32_t address, size_t size);
void hw_publish_io(Hw *hw, unsigned offset, unsigned size);
void hw_cartridge_write(Hw *hw, uint32_t address, uint16_t value);
uint16_t hw_cartridge_read(Hw *hw, uint32_t address);
void hw_flash_write(Hw *hw, uint16_t address, uint8_t value);
uint8_t hw_flash_read(Hw *hw, uint16_t address);
void hw_eeprom_write(Hw *hw, uint16_t bit);
uint16_t hw_eeprom_read(Hw *hw);
void hw_audio_io_write(Hw *hw, unsigned offset, uint16_t value, uint16_t mask);

#endif
