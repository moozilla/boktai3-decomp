#include "global.h"

void sub_08003734(u32 *);
void sub_080036F8(u32 *);

void sub_08003784(u32 *p)
{
    switch (*p) {
    case 1:
    case 3:
        sub_08003734(p);
        break;
    case 2:
    case 4:
        sub_080036F8(p);
        break;
    }
}
