#include "global.h"
#include "ram.h"

u8 *sub_08103F24(u8 *p)
{
    if (gNamedActorManager == 0)
        return 0;
    return p + 0x10c;
}
