#include "global.h"

void sub_0810E218(u8 *);
void Registry3C9F_Close(u8 *);

void Registry3C9F_OnCloseRequested(u8 *p)
{
    u8 *flag = p + 0xD2C;

    if (*flag) {
        sub_0810E218(p);
        Registry3C9F_Close(p);
        *flag = 0;
    }
}
