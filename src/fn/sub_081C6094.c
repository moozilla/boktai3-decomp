#include "challenge_result.h"
#include "input.h"
#include "engine_globals.h"
#include "ram.h"
void ChallengeResult_SetState(void *, void (*)(void *));
void TimeResult_UpdateResultHold(void *);
s32 sub_08249350(s32, s32);

void sub_081C6094(struct ChallengeTimeResult *p)
{
    s32 *counter = &p->timer;
    if (++*counter > 0x3c || (gPadState[1] & 1) != 0) {
        Sound_PlaySong(0x167);
        sub_081C62B4(p);
        ChallengeResult_SetState(p, TimeResult_UpdateResultHold);
    } else {
        struct SceneInputResource *resource = &p->digitGraphics;
        const u16 *table = gRandomTable;
        struct SceneInputWidget *digit = p->digits;
        s32 i;
        for (i = 0; i < 6; i++) {
            s32 r;
            gRandomIndex = (gRandomIndex + 1) & 0x3FF;
            do {
                r = table[gRandomIndex];
            } while (0);
            ChallengeResult_LoadDigit(digit, resource, sub_08249350(r >> 4, 10));
            digit++;
        }

        if (p->timer & 1) {
            Sound_PlaySong(0x273);
        }
    }
}
