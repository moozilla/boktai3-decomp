#include "global.h"
void sub_08245DE0(void);
void sub_08245630(u8, u16);
void sub_08245DA8(u8 a, u16 b)
{
    if (b == 0)
        sub_08245DE0();
    sub_08245630(a, b);
}
