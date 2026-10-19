#include "global.h"
#include "area_effect_link.h"

#ifdef NONMATCHING
// fuzzy: 96.7%
// foundry draft (worker best 96.7%); what differs is mostly register allocation and instruction order
#define LinkSession_UpdatePlayerValueAverage sub_081381CC
#define PlayerValue_GetBand                  Solar_GetLevelForIntensity
#define gLinkSession                         gUnk_020004B4
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
    player += index;
    base = (u32)session;
    base += 0x54;
    {
        u8 *field = (u8 *)(player + base);
        return *field;
    }
}

void sub_081381CC(struct AreaEffectLinkState *session)
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
#else
INCLUDE_ASM("asm/nonmatching", sub_081381CC);
#endif
