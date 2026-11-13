/* Portable SIIRTC entry points. Every GPIO edge reaches the cartridge device,
 * including edges that direct stores to wasm linear memory cannot express. */
#include "internal.h"
#include "hw_rtc.h"

enum { RTC_DATA = 0x080000c4, RTC_DIRECTION = 0x080000c6,
       RTC_ENABLE = 0x080000c8, RTC_PROTECTED = 0x030035c6 };

static uint8_t *protected_flag(Hw *hw)
{
    uint8_t *p = hw_map(hw, RTC_PROTECTED, 1);
    return p ? p : &hw->rtc_protected;
}

void Rtc_EnableGpioReads(void)
{
    Hw *hw = hw_bound();
    if (hw) hw_write16(hw, RTC_ENABLE, 1);
}
void Rtc_DisableGpioReads(void)
{
    Hw *hw = hw_bound();
    if (hw) hw_write16(hw, RTC_ENABLE, 0);
}
void Rtc_Initialize(void)
{
    Hw *hw = hw_bound();
    if (hw) { Rtc_EnableGpioReads(); *protected_flag(hw) = 0; }
}
void Rtc_Protect(void)
{
    Hw *hw = hw_bound();
    if (hw) { Rtc_DisableGpioReads(); *protected_flag(hw) = 1; }
}

uint32_t Rtc_WriteCommandByte(uint8_t value)
{
    Hw *hw = hw_bound();
    unsigned bit;
    if (!hw) return 0;
    for (bit = 0; bit < 8; ++bit) {
        uint16_t data = (uint16_t)(4 | ((value >> (7 - bit)) & 1) * 2);
        hw_write16(hw, RTC_DATA, data);
        hw_write16(hw, RTC_DATA, data | 1);
    }
    return 0;
}
uint32_t Rtc_WriteDataByte(uint8_t value)
{
    Hw *hw = hw_bound();
    unsigned bit;
    if (!hw) return 0;
    for (bit = 0; bit < 8; ++bit) {
        uint16_t data = (uint16_t)(4 | ((value >> bit) & 1) * 2);
        hw_write16(hw, RTC_DATA, data);
        hw_write16(hw, RTC_DATA, data | 1);
    }
    return 0;
}
uint8_t Rtc_ReadDataByte(void)
{
    Hw *hw = hw_bound();
    uint8_t value = 0;
    unsigned bit;
    if (!hw) return 0;
    for (bit = 0; bit < 8; ++bit) {
        hw_write16(hw, RTC_DATA, 4);
        hw_write16(hw, RTC_DATA, 5);
        value |= (uint8_t)(((hw_read16(hw, RTC_DATA) >> 1) & 1) << bit);
    }
    return value;
}

static int begin(uint8_t command)
{
    Hw *hw = hw_bound();
    if (!hw || *protected_flag(hw) == 1) return 0;
    *protected_flag(hw) = 1;
    hw_write16(hw, RTC_DATA, 1);
    hw_write16(hw, RTC_DATA, 5);
    hw_write16(hw, RTC_DIRECTION, 7);
    Rtc_WriteCommandByte(command);
    if (command & 1) hw_write16(hw, RTC_DIRECTION, 5);
    return 1;
}
static uint8_t end(void)
{
    Hw *hw = hw_bound();
    hw_write16(hw, RTC_DATA, 1);
    *protected_flag(hw) = 0;
    return 1;
}
static uint8_t read_record(uint8_t command, uint8_t *data, unsigned size)
{
    unsigned i;
    if (!data || !begin(command)) return 0;
    for (i = 0; i < size; ++i) data[i] = Rtc_ReadDataByte();
    return end();
}
static uint8_t write_record(uint8_t command, const uint8_t *data, unsigned size)
{
    unsigned i;
    if (!data || !begin(command)) return 0;
    for (i = 0; i < size; ++i) Rtc_WriteDataByte(data[i]);
    return end();
}
uint8_t Rtc_ReadDateTime(HwRtcRecord *record)
{
    uint8_t ok = read_record(0x65, (uint8_t *)record, 7);
    if (ok) record->hour &= 0x7f;
    return ok;
}
uint8_t Rtc_WriteDateTime(const HwRtcRecord *record)
{
    return write_record(0x64, (const uint8_t *)record, 7);
}
uint8_t Rtc_ReadTime(HwRtcRecord *record)
{
    uint8_t ok;
    if (!record) return 0;
    ok = read_record(0x67, &record->hour, 3);
    if (ok) record->hour &= 0x7f;
    return ok;
}
uint8_t sub_08248E70(const HwRtcRecord *record)
{
    return record ? write_record(0x66, &record->hour, 3) : 0;
}
uint8_t Rtc_ReadStatus(HwRtcRecord *record)
{
    uint8_t status;
    if (!record || !read_record(0x63, &status, 1)) return 0;
    record->status = (uint8_t)((status & 0xc0) | ((status & 0x20) >> 3) |
                             ((status & 8) >> 2) | ((status & 2) >> 1));
    return 1;
}
uint8_t Rtc_WriteStatus(const HwRtcRecord *record)
{
    uint8_t status;
    if (!record) return 0;
    status = (uint8_t)(0x40 | ((record->status & 4) << 3) |
                      ((record->status & 2) << 2) | ((record->status & 1) << 1));
    return write_record(0x62, &status, 1);
}
uint8_t sub_08248F0C(const HwRtcRecord *record)
{
    uint8_t alarm[2];
    if (!record) return 0;
    alarm[0] = record->alarm_hour;
    if ((alarm[0] >> 4) * 10 + (alarm[0] & 15) >= 12) alarm[0] |= 0x80;
    alarm[1] = record->alarm_minute;
    return write_record(0x68, alarm, 2);
}
uint8_t Rtc_Reset(void)
{
    HwRtcRecord record = {0};
    if (!begin(0x60)) return 0;
    end();
    record.status = 0x40;
    return Rtc_WriteStatus(&record);
}
uint8_t Rtc_CheckAndReset(void)
{
    HwRtcRecord record = {0};
    uint8_t errors = 0;
    if (!Rtc_ReadStatus(&record)) return 0;
    if ((record.status & 0xc0) == 0 || (record.status & 0xc0) == 0x80) {
        if (!Rtc_Reset()) return 0;
        ++errors;
    }
    Rtc_ReadTime(&record);
    if (record.second & 0x80) {
        if (!Rtc_Reset()) return (uint8_t)(errors << 4);
        ++errors;
    }
    return (uint8_t)((errors << 4) | 1);
}
