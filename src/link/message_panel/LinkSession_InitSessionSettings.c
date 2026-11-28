#include "global.h"
#include "scene/areas/area_effect_link.h"

void ClockPanel_CaptureSnapshot(struct AreaEffectLinkState *);
u32 SunClock_GetPhase(void);

void LinkSession_InitSessionSettings(struct AreaEffectLinkState *p)
{
    ClockPanel_CaptureSnapshot(p);
    *(u8 *)&p->exchangeValue = 0;
    p->sessionData[0x3D] = SunClock_GetPhase();
}
