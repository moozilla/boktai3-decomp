#include "global.h"

void sub_08055B90(void *, void (*)(void));
void sub_08055C04(void);
void sub_08055D38(void);

void sub_08055EB8(void *p, s32 m)
{
    if (m == 0)
        sub_08055B90(p, sub_08055C04);
    else if (m == 1)
        sub_08055B90(p, sub_08055D38);
    else
        sub_08055B90(p, sub_08055C04);
}
