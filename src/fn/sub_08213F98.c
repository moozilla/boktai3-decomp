#include "global.h"
void sub_08213F80(void);
void sub_08213F98(u16 n)
{
    u16 save = *(vu16 *)0x04000208;
    u16 i;
    *(vu16 *)0x04000208 = 1;
    i = n - 1;
    while (i != 0xFFFF) {
        sub_08213F80();
        i--;
    }
    *(vu16 *)0x04000208 = save;
}
