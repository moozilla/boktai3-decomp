#include "global.h"
void sub_081E4518(void);
void sub_081E44F8(u8 *p)
{
    *(u8 *)(p + 0xF88) = 0xc;
    *(void (**)(void))(p + 0xF98) = sub_081E4518;
}
