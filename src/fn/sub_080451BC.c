#include "encoded_record.h"
#include "engine.h"
#include "runtime.h"
/* EncodedRecord_EncodeAlternateFields: Packs game state and name into the alternate record format then applies seeded masking and writes its checksum */
#define EncodedRecord_EncodeAlternateFields sub_080451BC
struct EncodedRecordSession {
    u8 unknown0[0x18];
    u32 value;
};
s32 Div(s32, s32);
u32 Crc_ComputeCcitt16(const void *, u32, u32);
#define EncodedRecord_EncodeAlternateFields sub_080451BC
void EncodedRecord_EncodeAlternateFields(u8 *record)
{
    u32 bit;
    u32 randomIndex;
    s32 skip;
    s32 randomWord;
    s32 value;
    const u8 *name;
    u32 i;
    randomIndex = (gRandomIndex + 1) & 1023;
    gRandomIndex = randomIndex;
    randomWord = ((const u16 *)RANDOM_TABLE_ADDR)[randomIndex];
    skip = (randomWord >> 8) & 7;
    bit = 16;
    EncodedRecord_WriteBits(record, 0, &bit, 2, 6);
    EncodedRecord_WriteBits(record, 1, &bit, 3, 6);
    EncodedRecord_WriteBits(record, skip, &bit, 3, 6);
    EncodedRecord_WriteBits(record, ((struct EncodedRecordSession *)gSessionState)->value, &bit, 6,
                            6);
    EncodedRecord_WriteBits(record, gGameState->unk660, &bit, 3, 6);
    EncodedRecord_WriteBits(record, gGameState->unk662, &bit, 4, 6);
    EncodedRecord_WriteBits(record, (s16)gGameState->counter538 >> 3, &bit, 8, 6);
    EncodedRecord_WriteBits(record, gGameState->unk664, &bit, 7, 6);
    EncodedRecord_WriteBits(record, gGameState->unk7B4, &bit, 6, 6);
    EncodedRecord_WriteBits(record, 0, &bit, 6, 6);
    EncodedRecord_WriteBits(record, gGameState->unk520 >> 12, &bit, 10, 6);
    EncodedRecord_WriteBits(record, gGameState->unk51C >> 8, &bit, 8, 6);
    EncodedRecord_WriteBits(record, Div(gGameState->unk614, 216000), &bit, 6, 6);
    EncodedRecord_WriteBits(record, gGameState->unk536, &bit, 16, 6);
    name = gGameState->playerName;
    i = 0;
    if (*name) {
        name++;
        goto checkName;
    writeName:
        EncodedRecord_WriteBits(record, *name, &bit, 8, 6);
        name++;
        i++;
        if (i > 4)
            goto namesDone;
        if (*name == 0)
            goto padNames;
        name++;
    checkName:
        if (*name)
            goto writeName;
    }
padNames:
    while (i <= 4) {
        EncodedRecord_WriteBits(record, 0, &bit, 8, 6);
        i++;
    }
namesDone:
    EncodedRecord_WriteBits(record, 0, &bit, 144 - bit, 6);
    EncodedRecord_XorRange(record, 4, 24, 0xB8E6E, skip, 63);
    value = Crc_ComputeCcitt16(record, 3, 24);
    bit = 0;
    EncodedRecord_WriteBits(record, value, &bit, 16, 6);
}
