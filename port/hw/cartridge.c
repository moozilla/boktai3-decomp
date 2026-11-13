#include "internal.h"

static void dirty(Hw *hw)
{
    hw->save_dirty = 1;
    if (hw->host.save_dirty) hw->host.save_dirty(hw->host.user);
}

void hw_save_configure(Hw *hw, enum HwSaveKind kind)
{
    static const size_t sizes[] = {512, 8192, 65536, 131072};
    size_t i;
    if ((unsigned)kind > HW_SAVE_FLASH_128K) return;
    hw->save_kind = kind;
    hw->save_size = sizes[kind];
    for (i = 0; i < sizeof(hw->save); ++i) hw->save[i] = 0xff;
    hw->save_dirty = hw->flash_state = hw->flash_id = hw->flash_bank = 0;
    hw->eeprom_bits = 0;
    hw->eeprom_read_bit = 68;
}

int hw_save_load(Hw *hw, const void *bytes, size_t size)
{
    if (size != hw->save_size || !bytes) return HW_BAD_DATA;
    hw_copy(hw->save, bytes, size);
    hw->save_dirty = 0;
    return HW_OK;
}

const uint8_t *hw_save_data(const Hw *hw, size_t *size)
{
    if (size) *size = hw->save_size;
    return hw->save;
}

void hw_save_clean(Hw *hw) { hw->save_dirty = 0; }

uint8_t hw_flash_read(Hw *hw, uint16_t address)
{
    size_t offset = (size_t)hw->flash_bank * 65536 + address;
    if (hw->save_kind < HW_SAVE_FLASH_64K) return 0xff;
    if (hw->flash_id) return address == 0 ? 0xc2 : address == 1 ? (hw->save_size == 65536 ? 0x1c : 0x09) : 0xff;
    return offset < hw->save_size ? hw->save[offset] : 0xff;
}

void hw_flash_write(Hw *hw, uint16_t address, uint8_t value)
{
    size_t i, offset = (size_t)hw->flash_bank * 65536 + address;
    if (hw->save_kind < HW_SAVE_FLASH_64K) return;
    if (hw->flash_state == 3) {
        hw->save[offset] &= value;
        hw->flash_state = 0;
        dirty(hw);
        return;
    }
    if (value == 0xf0) { hw->flash_id = hw->flash_state = 0; return; }
    switch (hw->flash_state) {
    case 0: hw->flash_state = address == 0x5555 && value == 0xaa ? 1 : 0; break;
    case 1: hw->flash_state = address == 0x2aaa && value == 0x55 ? 2 : 0; break;
    case 2:
        hw->flash_state = 0;
        if (address != 0x5555) break;
        if (value == 0x90) hw->flash_id = 1;
        else if (value == 0xa0) hw->flash_state = 3;
        else if (value == 0x80) hw->flash_state = 4;
        else if (value == 0xb0) hw->flash_state = 7;
        break;
    case 4: hw->flash_state = address == 0x5555 && value == 0xaa ? 5 : 0; break;
    case 5: hw->flash_state = address == 0x2aaa && value == 0x55 ? 6 : 0; break;
    case 6:
        if (address == 0x5555 && value == 0x10) {
            for (i = 0; i < hw->save_size; ++i) hw->save[i] = 0xff;
            dirty(hw);
        } else if (value == 0x30) {
            offset &= ~(size_t)0xfff;
            for (i = 0; i < 4096; ++i) hw->save[offset + i] = 0xff;
            dirty(hw);
        }
        hw->flash_state = 0;
        break;
    case 7:
        if (!address && hw->save_size == 131072) hw->flash_bank = value & 1;
        hw->flash_state = 0;
        break;
    default: hw->flash_state = 0; break;
    }
}

void hw_eeprom_write(Hw *hw, uint16_t value)
{
    unsigned width = hw->save_kind == HW_SAVE_EEPROM_512 ? 6 : 14;
    unsigned bit = value & 1;
    unsigned pos = hw->eeprom_bits++;
    if (hw->save_kind > HW_SAVE_EEPROM_8K) return;
    if (pos == 0) {
        hw->eeprom_command = (uint8_t)bit;
        hw->eeprom_address = 0;
        hw_zero(hw->eeprom_buffer, 8);
        if (!bit) hw->eeprom_bits = 0;
    } else if (pos == 1) hw->eeprom_command = (uint8_t)(2 | bit);
    else if (pos < width + 2) hw->eeprom_address = (uint16_t)((hw->eeprom_address << 1) | bit);
    else if (hw->eeprom_command == 3) {
        hw->eeprom_address &= (uint16_t)(hw->save_size / 8 - 1);
        hw->eeprom_read_bit = 0;
        hw->eeprom_bits = 0;
    } else if (pos < width + 66) {
        unsigned data_bit = pos - width - 2;
        hw->eeprom_buffer[data_bit / 8] |= (uint8_t)(bit << (7 - data_bit % 8));
    } else {
        size_t address = (hw->eeprom_address & (hw->save_size / 8 - 1)) * 8;
        hw_copy(hw->save + address, hw->eeprom_buffer, 8);
        hw->eeprom_bits = 0;
        hw->eeprom_read_bit = 68;
        dirty(hw);
    }
}

uint16_t hw_eeprom_read(Hw *hw)
{
    unsigned bit = hw->eeprom_read_bit;
    if (bit >= 68) return 1;
    ++hw->eeprom_read_bit;
    if (bit < 4) return 0;
    bit -= 4;
    return (hw->save[hw->eeprom_address * 8 + bit / 8] >> (7 - bit % 8)) & 1;
}

static unsigned from_bcd(uint8_t v) { return (v >> 4) * 10 + (v & 15); }
static uint8_t to_bcd(unsigned v) { return (uint8_t)((v / 10) * 16 + v % 10); }
static int leap(unsigned y) { return y % 4 == 0 && (y % 100 != 0 || y % 400 == 0); }
static unsigned month_days(unsigned year, unsigned month)
{
    static const uint8_t days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    return days[month - 1] + (month == 2 && leap(year));
}

static int64_t host_time(Hw *hw)
{
    /* Deterministic default is 2000-01-01. The host supplies wall-clock time. */
    return hw->host.unix_time ? hw->host.unix_time(hw->host.user) : INT64_C(946684800);
}

void hw_rtc_datetime(Hw *hw, uint8_t bcd[7])
{
    int64_t seconds = host_time(hw) + hw->rtc_offset;
    int64_t days;
    unsigned year = 1970, month = 1;
    if (seconds < 0) seconds = 0;
    days = seconds / 86400;
    bcd[3] = (uint8_t)((days + 4) % 7);
    bcd[4] = to_bcd((unsigned)(seconds / 3600 % 24));
    bcd[5] = to_bcd((unsigned)(seconds / 60 % 60));
    bcd[6] = to_bcd((unsigned)(seconds % 60));
    /* The cartridge date range is a two-digit year. Limit pathological hosts. */
    days %= 146097; /* Gregorian calendar repeats every 400 years. */
    while (days >= 365 + leap(year)) days -= 365 + leap(year++);
    while (days >= month_days(year, month)) days -= month_days(year, month++);
    bcd[0] = to_bcd(year % 100);
    bcd[1] = to_bcd(month);
    bcd[2] = to_bcd((unsigned)days + 1);
}

int hw_rtc_set_datetime(Hw *hw, const uint8_t bcd[7])
{
    unsigned i, year = 2000 + from_bcd(bcd[0]), month = from_bcd(bcd[1]);
    unsigned day = from_bcd(bcd[2]), hour = from_bcd(bcd[4] & 0x7f);
    unsigned minute = from_bcd(bcd[5]), second = from_bcd(bcd[6]);
    int64_t days = 0;
    for (i = 0; i < 7; ++i)
        if ((bcd[i] & 15) > 9 || ((bcd[i] & (i == 4 ? 0x70 : 0xf0)) >> 4) > 9) return HW_BAD_DATA;
    if (!month || month > 12 || !day || day > month_days(year, month) || hour > 23 || minute > 59 || second > 59 || bcd[3] > 6) return HW_BAD_DATA;
    for (i = 1970; i < year; ++i) days += 365 + leap(i);
    for (i = 1; i < month; ++i) days += month_days(year, i);
    days += day - 1;
    hw->rtc_offset = days * 86400 + hour * 3600 + minute * 60 + second - host_time(hw);
    return HW_OK;
}

void hw_set_solar(Hw *hw, unsigned threshold)
{
    hw->solar_threshold = (uint16_t)(threshold > 255 ? 255 : threshold);
}

static void rtc_command(Hw *hw)
{
    unsigned command = hw->rtc_command;
    hw->rtc_read = command & 1;
    hw->rtc_count = 0;
    if ((command & 0xf0) != 0x60) { hw->rtc_active = 0; return; }
    switch (command & 0x0e) {
    case 0: hw->rtc_status = 0x40; hw->rtc_active = 0; break;
    case 2: hw->rtc_data[0] = hw->rtc_status; hw->rtc_count = 1; break;
    case 4: hw_rtc_datetime(hw, hw->rtc_data); hw->rtc_count = 7; break;
    case 6: {
        uint8_t date[7];
        hw_rtc_datetime(hw, date);
        hw_copy(hw->rtc_data, date + 4, 3);
        hw->rtc_count = 3;
        break;
    }
    default: hw->rtc_active = 0; break;
    }
    if (!hw->rtc_read) hw_zero(hw->rtc_data, sizeof(hw->rtc_data));
}

static void rtc_finish_write(Hw *hw)
{
    switch (hw->rtc_command & 0x0e) {
    case 2: hw->rtc_status = hw->rtc_data[0]; break;
    case 4: (void)hw_rtc_set_datetime(hw, hw->rtc_data); break;
    case 6: {
        uint8_t date[7];
        hw_rtc_datetime(hw, date);
        hw_copy(date + 4, hw->rtc_data, 3);
        (void)hw_rtc_set_datetime(hw, date);
        break;
    }
    }
}

void hw_cartridge_write(Hw *hw, uint32_t address, uint16_t value)
{
    uint16_t old = hw->gpio_data;
    if (address == 0x080000c6) { hw->gpio_direction = value & 15; return; }
    if (address == 0x080000c8) { hw->gpio_enable = value & 1; return; }
    if (address != 0x080000c4) return;
    hw->gpio_data = value & 15;
    if (!(value & 4)) {
        hw->rtc_active = 0;
        if (value & 2) hw->solar_clocks = 0;
        else if ((value & 1) && !(old & 1) && hw->solar_clocks < 0xffff) ++hw->solar_clocks;
        hw->solar_output = !(value & 2) && hw->solar_clocks >= hw->solar_threshold;
        return;
    }
    if (!(old & 4)) {
        hw->rtc_active = 1; hw->rtc_bits = hw->rtc_command = 0;
        return;
    }
    if (!hw->rtc_active || !(value & 1) || (old & 1)) return;
    if (hw->rtc_bits < 8) {
        hw->rtc_command = (uint8_t)((hw->rtc_command << 1) | ((value >> 1) & 1));
        if (++hw->rtc_bits == 8) rtc_command(hw);
    } else if (hw->rtc_read) {
        /* A read presents its bit on this rising edge until the next one. */
        unsigned bit = hw->rtc_bits++ - 8;
        if (bit < (unsigned)hw->rtc_count * 8) {
            hw->gpio_data = (uint16_t)((hw->gpio_data & ~2u) | ((hw->rtc_data[bit / 8] >> (bit % 8)) & 1) * 2);
        }
    } else {
        unsigned bit = hw->rtc_bits++ - 8;
        if (bit < (unsigned)hw->rtc_count * 8) hw->rtc_data[bit / 8] |= (uint8_t)(((value >> 1) & 1) << (bit % 8));
        if (hw->rtc_bits == 8 + hw->rtc_count * 8) { rtc_finish_write(hw); hw->rtc_active = 0; }
    }
}

uint16_t hw_cartridge_read(Hw *hw, uint32_t address)
{
    if (address == 0x080000c6) return hw->gpio_direction;
    if (address == 0x080000c8) return hw->gpio_enable;
    return (uint16_t)((hw->gpio_data & 7) | (hw->solar_output ? 8 : 0));
}

/* The SDK sends the most significant bit of data[3] first. Save-file bytes
 * preserve that wire order, as the emulator's EEPROM backing store does. */
uint32_t Eeprom_SetCapacity(uint16_t kilobits)
{
    Hw *hw = hw_bound();
    enum HwSaveKind kind = kilobits == 64 ? HW_SAVE_EEPROM_8K : HW_SAVE_EEPROM_512;
    if (!hw) return 1;
    if (hw->save_kind != kind) hw_save_configure(hw, kind);
    return kilobits != 4 && kilobits != 64;
}

uint16_t Eeprom_ReadBlock(uint16_t address, uint16_t *data)
{
    Hw *hw = hw_bound();
    unsigned i;
    if (!hw || !data || hw->save_kind > HW_SAVE_EEPROM_8K || address >= hw->save_size / 8) return 0x80ff;
    for (i = 0; i < 4; ++i) data[3 - i] = (uint16_t)((uint16_t)hw->save[address * 8 + i * 2] << 8 | hw->save[address * 8 + i * 2 + 1]);
    return 0;
}

uint16_t Eeprom_WriteBlock(uint16_t address, const uint16_t *data, uint8_t wait)
{
    Hw *hw = hw_bound();
    unsigned i;
    (void)wait;
    if (!hw || !data || hw->save_kind > HW_SAVE_EEPROM_8K || address >= hw->save_size / 8) return 0x80ff;
    for (i = 0; i < 4; ++i) {
        hw->save[address * 8 + i * 2] = (uint8_t)(data[3 - i] >> 8);
        hw->save[address * 8 + i * 2 + 1] = (uint8_t)data[3 - i];
    }
    dirty(hw);
    return 0;
}

uint16_t Eeprom_VerifyBlock(uint16_t address, const uint16_t *data)
{
    uint16_t actual[4], error = Eeprom_ReadBlock(address, actual);
    unsigned i;
    if (error) return error;
    for (i = 0; i < 4; ++i) if (actual[i] != data[i]) return 0x8000;
    return 0;
}

uint16_t Eeprom_WriteBlockWithFullDelay(uint16_t a, const uint16_t *d) { return Eeprom_WriteBlock(a, d, 1); }
uint16_t sub_08248930(uint16_t a, const uint16_t *d) { return Eeprom_WriteBlock(a, d, 1); }
