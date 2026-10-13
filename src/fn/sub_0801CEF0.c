#include "global.h"
int sub_0801B54C(void *);
void ResultMenu_SendCommand(void *, int, int, void *);
void sub_0822AE7C(int);
void sub_0801B534(void *, void *);
void Transformation_Charge(void *);
void ResultGame_Finish(u8 *p)
{
    if (sub_0801B54C(p)) {
        ResultMenu_SendCommand(p, 0x37, 0, 0);
        sub_0822AE7C(0xa9);
    }
    if (*(u32 *)(p + 0x3c) > 29)
        sub_0801B534(p, Transformation_Charge);
    else
        ++*(u32 *)(p + 0x3c);
}
