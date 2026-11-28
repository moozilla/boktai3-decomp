#include "link/encoded_records/encoded_record.h"
#include "engine/engine.h"
#include "engine/runtime.h"
struct EncodedRecordSession {
    u8 unknown0[0x18];
    u32 value;
};
s32 Div(s32, s32);
u32 Crc_ComputeIbm16(const void *, u32, u32);
void EncodedRecord_EncodeFields(u8 *record)
{
    u32 bit;
    s32 skip;
    u32 value;
    const u8 *name;
    u32 i;
    s32 random;

    gRandomIndex = (gRandomIndex + 1) & 1023;
    random = ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
    skip = (random >> 8) & 7;
    bit = 16;
    EncodedRecord_WriteBits(record, 0, &bit, 2, 6);
    EncodedRecord_WriteBits(record, 1, &bit, 3, 6);
    EncodedRecord_WriteBits(record, skip, &bit, 3, 6);
    EncodedRecord_WriteBits(record, ((struct EncodedRecordSession *)gSessionState)->value, &bit, 6,
                            6);
    value = (s16)gGameState->counter538 >> 3;
    if (value > 255)
        value = 255;
    EncodedRecord_WriteBits(record, value, &bit, 8, 6);
    value = gGameState->unk664;
    if (value > 127)
        value = 127;
    EncodedRecord_WriteBits(record, value, &bit, 7, 6);
    value = gGameState->unk75A;
    if (value > 511)
        value = 511;
    EncodedRecord_WriteBits(record, value, &bit, 9, 6);
    value = gGameState->unk7B4;
    if (value > 127)
        value = 127;
    EncodedRecord_WriteBits(record, value, &bit, 7, 6);
    value = gGameState->unk75F;
    if (value > 1)
        value = 1;
    EncodedRecord_WriteBits(record, value, &bit, 1, 6);
    EncodedRecord_WriteBits(record, gGameState->unk75E, &bit, 6, 6);
    value = gGameState->unk520 >> 12;
    if (value > 1023)
        value = 1023;
    EncodedRecord_WriteBits(record, value, &bit, 10, 6);
    value = gGameState->unk51C >> 8;
    if (value > 255)
        value = 255;
    EncodedRecord_WriteBits(record, value, &bit, 8, 6);
    value = Div(gGameState->unk614, 216000);
    if (value > 63)
        value = 63;
    EncodedRecord_WriteBits(record, value, &bit, 6, 6);
    EncodedRecord_WriteBits(record, gGameState->unk536, &bit, 12, 6);
    name = gGameState->playerName;
    for (i = 0; i < 5 && *name; i++) {
        name++;
        if (*name == 0)
            break;
        EncodedRecord_WriteBits(record, *name, &bit, 8, 6);
        name++;
    }
    for (; i < 5; i++)
        EncodedRecord_WriteBits(record, 0, &bit, 8, 6);
    EncodedRecord_WriteBits(record, 0, &bit, 144 - bit, 6);
    EncodedRecord_XorRange(record, 4, 24, 0x8C159, skip, 63);
    value = Crc_ComputeIbm16(record, 3, 24);
    bit = 0;
    EncodedRecord_WriteBits(record, value, &bit, 16, 6);
}
