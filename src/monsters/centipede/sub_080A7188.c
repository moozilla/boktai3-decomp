#include "global.h"
/* CentipedeBody_ClearStateEntryFlag: Clears the pending state-entry byte when set */

void CentipedeBody_ClearStateEntryFlag(u8 *p)
{
    if (p[0x2b2])
        p[0x2b2] = 0;
}
