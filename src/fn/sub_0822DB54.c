#include "global.h"
#include "engine.h"
/* 32-byte records of the table at 08E6092C */
struct Record {
    u8 pad0[0xC];
    u8 a, aList[3];
    u8 b, bList[3];
    u8 pad14;
    u8 hValues[3];
    u8 pad18[8];
};
struct RecordEntry {
    u8 pad0[2];
    u16 d;
    u8 a, aList[3];
    u8 b, bList[3];
    u8 h, hValue, fieldE, fieldF;
};
extern const struct Record gUnk_08E6092C[];
s32 sub_0822D87C(struct Record *record);
struct RecordEntry *sub_0822D6C8(void);
/* Script: copies record -w of 08E6092C into a new entry (sub_0822D6C8):
   with -s the a/b lists too, -H (0-2) picks one of three values, -d
   (default 1000). Returns 1, or 0 on failure. */
s32 RecordEntry_AddFromScript(void)
{
    struct Record record;
    struct RecordEntry *entry;
    s32 h, i;
    if (!Script_SeekToKeyword('w'))
        return 0;
    record = gUnk_08E6092C[Script_GetValue()];
    if (sub_0822D87C(&record) < 0)
        return 0;
    entry = sub_0822D6C8();
    if (!entry)
        return 0;
    if (Script_GetKeywordValue('s', 0)) {
        s32 count = 3;
        entry->a = record.a;
        entry->b = record.b;
        for (i = 0; i < count; i++) {
            entry->aList[i] = record.aList[i];
            entry->bList[i] = record.bList[i];
        }
    }
    h = Script_GetKeywordValue('H', 0);
    if (h < 0)
        h = 0;
    else if (h > 2)
        h = 2;
    entry->h = h;
    entry->hValue = record.hValues[h];
    entry->fieldE = 0;
    entry->fieldF = 0;
    entry->d = Script_GetKeywordValue('d', 1000);
    return 1;
}
