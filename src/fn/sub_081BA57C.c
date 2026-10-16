#include "global.h"
#include "engine.h"
extern u8 *gUnk_0200025C;
void sub_081BB150(void *);
void sub_081BB13C(void *);
/* Script: for the registry 2388 task (gUnk_0200025C) with a child at +0x558:
   -v nonzero clears +0x52A and calls sub_081BB150, else sets it and calls
   sub_081BB13C (tentative). */
void Registry2388_SetVisibleFromScript(void)
{
    u8 *p = gUnk_0200025C;
    void **child;
    if (!p)
        return;
    child = (void **)(p + 0x558);
    if (!*child)
        return;
    if (Script_GetKeywordValue('v', 0)) {
        *(u16 *)(p + 0x52A) = 0;
        sub_081BB150(*child);
    } else {
        *(u16 *)(p + 0x52A) = 1;
        sub_081BB13C(*child);
    }
}
