#include "global.h"
#include "engine.h"
/* 32-byte records of the table at 08E6092C */
struct Record {
    u32 words[8];
};
extern const struct Record gUnk_08E6092C[];
void sub_0822D858(s32 index);
void sub_0822D904(struct Record *record);
/* game state fields, addressed index-first as the original does */
#define STATE_U16(offset) (*(u16 *)((u8 *)gUnk_02000710 + (offset)))
#define SLOT68(i)         (*(s16 *)((u8 *)gUnk_02000710 + (i) * 2 + 0x68))
/* Script: clears game state +0x72C (two words), the 48 entries reset by
   sub_0822D858, the eight slots at +0x68 and +0x50, then -n count, -w count
   record ids (each record of 08E6092C copied and passed to sub_0822D904) and
   -q eight slot values. */
void GameState_ResetEntriesFromScript(void)
{
    s32 i, count;
    struct Record record;
    for (i = 0; i <= 1; i++)
        *(u32 *)((u8 *)gUnk_02000710 + i * 4 + 0x72C) = 0;
    for (i = 0; i <= 47; i++)
        sub_0822D858(i);
    for (i = 0; i <= 7; i++)
        SLOT68(i) = -1;
    STATE_U16(0x50) = 0;
    if (Script_SeekToKeyword('n'))
        count = Script_GetValue();
    else
        count = 0;
    if (count > 0) {
        if (Script_SeekToKeyword('w')) {
            for (i = 0; i < count; i++) {
                record = gUnk_08E6092C[Script_GetValue()];
                sub_0822D904(&record);
            }
        }
        if (Script_SeekToKeyword('q'))
            for (i = 0; i <= 7; i++)
                SLOT68(i) = Script_GetValue();
    }
}
