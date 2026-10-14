#ifndef GUARD_ENCODED_RECORD_H
#define GUARD_ENCODED_RECORD_H
#include "game_state.h"
/* Tentative decoded record layout: meanings follow the first encoder. */
struct EncodedRecordFields {
    u32 sessionValue;
    u32 counter538;
    u32 value664, value75A, value7B4, value75F, value75E;
    u32 value520, value51C, elapsedUnits, value536;
    u8 name[5];
};
struct EncodedAlternateRecordFields {
    u32 sessionValue;
    u32 value660, value662, counter538, value664, value7B4, reserved18;
    u32 value520, value51C, elapsedUnits, value536;
    u8 name[5];
};
void EncodedRecord_WriteBits(u8 *, s32, u32 *, s32, s32);
u32 EncodedRecord_ReadBits(const u8 *, u32 *, s32, s32);
void EncodedRecord_XorRange(u8 *, s32, s32, u32, s32, u32);
#endif
