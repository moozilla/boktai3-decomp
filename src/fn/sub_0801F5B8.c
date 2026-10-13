// MATCHING_CONCESSION: register-pinning
#include "global.h"
void sub_082260A4(int);
void Script_Call(int, int);
void SceneObject_UpdateLowered(u8 *s)
{
    register int timer asm("r6") = *(s16 *)(s + 0x82);
    if (timer) {
        sub_082260A4(timer);
        (*(u16 *)(s + 0x82))--;
    } else if (*(int *)(s + 0x64)) {
        Script_Call(*(int *)(s + 0x64), 0);
        *(int *)(s + 0x64) = timer;
    }
}
