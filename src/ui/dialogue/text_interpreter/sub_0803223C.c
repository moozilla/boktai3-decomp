#include "global.h"

u8 *Text_FindChar(u8 *s, u8 c)
{
    while (*s != c && *s != 0)
        s++;
    return s;
}
