#ifndef GUARD_WIRELESS_TEXT_H
#define GUARD_WIRELESS_TEXT_H

#include "engine.h"

u16 *WirelessText_GetCharacterTable(void);
u16 WirelessText_GetCharacterCode(u8 i);
u8 WirelessText_FindCharacterIndex(u32 character);
void WirelessText_DecodeString(const u8 *source, u8 *destination, int count);
void WirelessText_EncodeString(const u8 *source, u8 *destination, int count);
void WirelessText_InitCharacterTableFromScript(void);

#endif
