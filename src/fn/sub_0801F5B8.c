#include "global.h"
void Camera_RequestShake(int);
void Script_Call(int, int);
void SceneObject_UpdateLowered(u8 *s)
{
    int timer = *(s16 *)(s + 0x82);
    if (timer) {
        Camera_RequestShake(timer);
        do {
        (*(u16 *)(s + 0x82))--;
        } while (0);
    } else {
        do {
            if (*((int *)(s + 0x64))) {
        Script_Call(*(int *)(s + 0x64), 0);
        *(int *)(s + 0x64) = timer;
            }
        } while (0);
    }
}
