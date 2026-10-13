#include "global.h"
int sub_0801B54C(void *);
void ResultMenu_SendCommand(void *, int, int, void *);
void sub_0801B534(void *, void *);
void ResultGame_Update(void *);
void sub_0822B2F8(int);
void ResultMenu_Countdown(u8 *p)
{
    if (sub_0801B54C(p))
        ResultMenu_SendCommand(p, 0x31, 0, 0);
    if (*(u32 *)(p + 0x3c) > 239)
        sub_0801B534(p, ResultGame_Update);
    else {
        if (Mod(*(u32 *)(p + 0x3c), 61) == 60)
            sub_0822B2F8(0x21c);
        ++*(u32 *)(p + 0x3c);
    }
}
