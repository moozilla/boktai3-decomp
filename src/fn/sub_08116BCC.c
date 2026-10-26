#include "global.h"
extern u8 *gGameState;
struct S {
    u8 f[0x74];
    u32 a;
};
extern struct S *gUnk_020004F0;
void Player_SaveCurrentHealth(void)
{
    if (gUnk_020004F0)
        *(u16 *)(gGameState + 0x758) = gUnk_020004F0->a;
}
