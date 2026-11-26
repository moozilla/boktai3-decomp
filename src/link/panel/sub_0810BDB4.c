#include "global.h"
#include "engine/engine.h"
#include "scene/areas/area_effect_link.h"
struct TimerObject {
    u8 pad0[0x53];
    u8 state;
    u8 pad54[0x1F8];
    s32 times[3];
};
/* Script: for the object at 0x020004B4 in state 1 (+0x53), stores -t (clamped
   to 60000) in slot -p (0-2) of the words at +0x24C (tentative). */
void RegistryC5BD_SetTimeFromScript(void)
{
    u32 slot = Script_GetKeywordValue('p', -1);
    s32 t;
    struct TimerObject *p;
    if (slot > 2)
        return;
    t = Script_GetKeywordValue('t', -1);
    if (t < 0)
        return;
    if (t > 60000)
        t = 60000;
    p = gLinkSession;
    if (p && p->state == 1)
        p->times[slot] = t;
}
