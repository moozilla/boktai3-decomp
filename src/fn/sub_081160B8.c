#include "global.h"
void sub_08115DD0(void);
void sub_081160B8(u8 *p)
{
    *(void (**)(void))(p + 0xb4) = sub_08115DD0;
    p[0xb1] = 1;
}
