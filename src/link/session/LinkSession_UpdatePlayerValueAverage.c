#include "global.h"
#include "scene/areas/area_effect_link.h"

#define PlayerValue_GetBand Solar_GetLevelForIntensity
s32 PlayerValue_GetBand(s32);
s32 Div(s32, s32);
static inline u16 GetSelection(s32 player)
{
    struct AreaEffectLinkState *session = gLinkSession;
    if (!session)
        return 0;
    {
        s32 index = player + session->previousSelectionPage * 4;
        return session->selectionBits[index];
    }
}

void LinkSession_UpdatePlayerValueAverage(struct AreaEffectLinkState *session)
{
    s32 i;
    session->playerValueSum = 0;
    for (i = 0; i < session->participantCount; i++) {
        s32 value = GetSelection(i) * 4;
        session->playerValues[i] = PlayerValue_GetBand(value);
        value += session->playerValueSum;
        session->playerValueSum = value;
    }
    if (session->participantCount == 0)
        session->value = 0;
    else
        session->value = Div(session->playerValueSum, session->participantCount);
}
