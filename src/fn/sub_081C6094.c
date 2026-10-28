#include "global.h"
#include "input.h"
#include "engine_globals.h"
#include "ram.h"
void Sound_PlaySong(u32);
void sub_081C62B4(u8 *);
void ChallengeResult_SetState(void *, void (*)(void *));
void TimeResult_UpdateResultHold(void *);
void ChallengeResult_LoadDigit(u8 *, void *, s32);
s32 sub_08249350(s32, s32);

void sub_081C6094(u8 *p)
{
    s32 *counter = (s32 *)(p + 0x370);
    if (++*counter > 0x3c || (gPadState[1] & 1) != 0) {
        Sound_PlaySong(0x167);
        sub_081C62B4(p);
        ChallengeResult_SetState(p, TimeResult_UpdateResultHold);
    } else {
        u8 *resource = p + 0x38;
        const u16 *table = gRandomTable;
        u8 *digit = p + 0x118;
        s32 i;
        for (i = 0; i < 6; i++) {
            s32 r;
            gRandomIndex = (gRandomIndex + 1) & 0x3FF;
            do {
                r = table[gRandomIndex];
            } while (0);
            ChallengeResult_LoadDigit(digit, resource, sub_08249350(r >> 4, 10));
            digit += 0x60;
        }

        if ((*((s32 *)(p + 0x370))) & 1) {
            Sound_PlaySong(0x273);
        }
    }
}
