// MATCHING_CONCESSION: register-pinning
#include "global.h"
void Camera_RequestShake(int);
void Script_Call(int, int);
void SceneObject_UpdateLowered(u8 *s)
{
    register int timer asm("r6") = *(s16 *)(s + 0x82);
    if (timer) {
        Camera_RequestShake(timer);
        (*(u16 *)(s + 0x82))--;
    } else if (*(int *)(s + 0x64)) {
        Script_Call(*(int *)(s + 0x64), 0);
        *(int *)(s + 0x64) = timer;
    }
}
