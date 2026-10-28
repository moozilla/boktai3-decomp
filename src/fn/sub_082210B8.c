#include "global.h"
#include "ram.h"
#include "engine_globals.h"
s32 Random_Next15(void);
void Random_InitValueTable(void)
{
    s32 i;
    for (i = 0; i <= 1023; i++) {
        s32 value = Random_Next15();
        do {
            gRandomTable[i] = value >> (Random_Next15() & 3);
        } while (0);
    }
    gRandomIndex = Random_Next15() & 1023;
    gEffectRandomTableIndex = Random_Next15() & 1023;
}
