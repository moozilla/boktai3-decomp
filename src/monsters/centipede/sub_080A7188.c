#include "global.h"
/* TentacleBody_ClearStateEntryFlag: Clears the pending state-entry byte when set */
#define TentacleBody_ClearStateEntryFlag sub_080A7188

void sub_080A7188(u8 *p)
{
    if (p[0x2b2])
        p[0x2b2] = 0;
}
