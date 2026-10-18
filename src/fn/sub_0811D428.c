#include "global.h"
/* Tentative style descriptor embedded at +0x48 by 0803BBE4. */
struct SceneActorStyle {
    u8 adjustedA, adjustedB, adjustedC, parameterD, parameterE, parameterA;
    u8 unknown6[2], entries[4], count, parameterB, parameterF, parameterC;
    s16 initialHalfword, currentHalfword;
    u32 initialWord, currentWord;
};
void SceneActorStyle_Initialize(struct SceneActorStyle *s, s32 a, s32 b, s32 c, s32 e, s32 d, s32 f, s32 halfword,
                  u32 word)
{
    s32 i;
    u8 *entry;
    s->adjustedA = a + 12;
    s->adjustedB = b + 6;
    s->adjustedC = c + 18;
    s->parameterE = e;
    {
        u32 zero = 0;
        i = 3;
        entry = &s->entries[3];
        do {
            *entry-- = zero;
        } while (--i >= 0);
    }
    s->count = 0;
    s->parameterD = d;
    s->parameterB = b;
    s->parameterF = f;
    s->parameterC = c;
    s->parameterA = a;
    s->initialHalfword = halfword;
    s->currentHalfword = halfword;
    s->initialWord = word;
    s->currentWord = word;
}
