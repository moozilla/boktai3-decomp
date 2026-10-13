#include "global.h"

void sub_08040BF8(u8 *);
void sub_08040CF4(u8 *);

void sub_08040FC0(u8 *p)
{
    switch (*(s32 *)(p + 0x18)) {
    case 0:
    case 2:
        sub_08040BF8(p);
        break;
    case 1:
        sub_08040CF4(p);
        break;
    }
}
