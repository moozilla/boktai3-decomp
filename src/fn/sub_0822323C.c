#include "global.h"

void sub_08223128(void);
void Serial_EnableInterrupt(void);

s32 Serial_DetectRoleAndEnableInterrupt(void)
{
    s32 r;
    sub_08223128();
    r = *(s32 *)0x03005324;
    if (r >= 0) {
        if (*(s32 *)0x0300531C != 0) {
            Serial_EnableInterrupt();
            r = 1;
        } else {
            Serial_EnableInterrupt();
            r = 0;
        }
    }
    return r;
}
