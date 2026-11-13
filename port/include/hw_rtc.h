#ifndef BOKTAI_PORT_HW_RTC_H
#define BOKTAI_PORT_HW_RTC_H
#include <stdint.h>

/* Byte layout used by the original SIIRTC SDK and the game's RTC records. */
typedef struct HwRtcRecord {
    uint8_t year, month, day, weekday;
    uint8_t hour, minute, second, status;
    uint8_t alarm_hour, alarm_minute;
} HwRtcRecord;

void Rtc_Initialize(void);
void Rtc_Protect(void);
void Rtc_EnableGpioReads(void);
void Rtc_DisableGpioReads(void);
uint32_t Rtc_WriteCommandByte(uint8_t value);
uint32_t Rtc_WriteDataByte(uint8_t value);
uint8_t Rtc_ReadDataByte(void);
uint8_t Rtc_ReadDateTime(HwRtcRecord *record);
uint8_t Rtc_WriteDateTime(const HwRtcRecord *record);
uint8_t Rtc_ReadTime(HwRtcRecord *record);
uint8_t sub_08248E70(const HwRtcRecord *record); /* SDK write time */
uint8_t Rtc_ReadStatus(HwRtcRecord *record);
uint8_t Rtc_WriteStatus(const HwRtcRecord *record);
uint8_t sub_08248F0C(const HwRtcRecord *record); /* SDK write alarm */
uint8_t Rtc_Reset(void);
uint8_t Rtc_CheckAndReset(void);
#endif
