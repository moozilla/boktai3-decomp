#include "global.h"
void sub_082454D4(void);
void sub_08245630(u8, u16);
void sub_082458E0(u8 a, u16 b)
{
    if (b == 0)
        sub_082454D4();
    sub_08245630(a, b);
}
