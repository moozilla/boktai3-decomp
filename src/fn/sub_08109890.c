#include "global.h"

void LinkConnectionMenu_RunExitScript(u8 *);

void LinkConnectionMenu_OnExitState(u8 *p)
{
    if (p[0x35] == 1) {
        LinkConnectionMenu_RunExitScript(p);
        p[0x35] = 0;
    }
}
