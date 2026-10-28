// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "area_effect_link.h"

// foundry draft (worker best 96.7%); what differs is mostly register allocation and instruction order
#define PlayerValue_GetBand Solar_GetLevelForIntensity
s32 PlayerValue_GetBand(s32);
s32 Div(s32, s32);
static inline u32 GetSelection(s32 player)
{
    struct AreaEffectLinkState *session = gLinkSession;
    u32 base;
    u32 index;
    if (!session)
        return 0;
    index = session->previousSelectionPage;
    index <<= 2;
    {
        u32 sum = player + index;
        asm("" : "+r"(sum));
        base = (u32)session + 0x54;
        return *(u8 *)(base + sum);
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
