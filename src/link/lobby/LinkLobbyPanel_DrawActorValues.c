#include "global.h"
#include "scene/areas/area_effect_link.h"

void PanelText_DrawDigits(s32, s32, s32, s32, s32);

void LinkLobbyPanel_DrawActorValues(void)
{
    struct AreaEffectLinkState *session = gLinkSession;

    if (!session) {
        PanelText_DrawDigits(0, 100, 0, 18, 0);
        PanelText_DrawDigits(0, 100, 18, 18, 0);
    } else {
        PanelText_DrawDigits(session->actorValues[0], 100, 0, 18, 0);
        PanelText_DrawDigits(session->actorValues[1], 100, 18, 18, 0);
    }
}
