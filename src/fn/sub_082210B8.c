#include "global.h"
extern s16 gUnk_0203B400[1024];
extern u32 gUnk_03005308, gUnk_03005304;
s32 Random_Next15(void);
void Random_InitValueTable(void)
{
    s32 i;
    for (i = 0; i <= 1023; i++) {
        s32 value = Random_Next15();
        do {
            gUnk_0203B400[i] = value >> (Random_Next15() & 3);
        } while (0);
    }
    gUnk_03005308 = Random_Next15() & 1023;
    gUnk_03005304 = Random_Next15() & 1023;
}
