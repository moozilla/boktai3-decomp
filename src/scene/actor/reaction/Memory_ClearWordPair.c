#include "global.h"

/* The initializer callers consume the preserved incoming pointer as status. */
u32 *Memory_ClearWordPair(u32 *p)
{
    p[0] = 0;
    p[1] = 0;
    return p;
}
