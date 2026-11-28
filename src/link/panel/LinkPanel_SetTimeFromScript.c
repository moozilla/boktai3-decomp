#include "global.h"
#include "engine/engine.h"
#include "scene/areas/area_effect_link.h"

/* Script: stores a player's record time 't' (at most 60000) in slot 'p' (0 or 1,
   or 2) of the link session's record times. */
void LinkPanel_SetTimeFromScript(void)
{
    u32 slot = Script_GetKeywordValue('p', -1);
    s32 time;

    if (slot > 2)
        return;
    time = Script_GetKeywordValue('t', -1);
    if (time < 0)
        return;
    if (time > 60000)
        time = 60000;
    if (gLinkSession && gLinkSession->unavailable == 1)
        gLinkSession->recordTicks[slot] = time;
}
