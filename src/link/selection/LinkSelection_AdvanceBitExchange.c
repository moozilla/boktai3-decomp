#define LinkSelection_SwapPages LinkSession_ToggleConnectionRole
#include "global.h"
#include "scene/areas/area_effect_link.h"
#include "system/game_state/game_state.h"
void LinkSession_ToggleConnectionRole(struct AreaEffectLinkState *);
void LinkSession_UpdatePlayerValueAverage(struct AreaEffectLinkState *);
static inline u8 ReceiveBit(void)
{
    struct AreaEffectLinkState *p = gLinkSession;
    if (!p)
        return 0;
    return (p->receiveBits >> p->receiveBit) & 1;
}
void LinkSelection_AdvanceBitExchange(struct AreaEffectLinkState *p, u32 flags, u16 *value)
{
    s32 i;
    if (flags & 1)
        p->sendBit++;
    if (flags & 2) {
        LinkSession_ToggleConnectionRole(p);
        LinkSession_UpdatePlayerValueAverage(p);
        p->sendBit = 0;
        p->receiveBits = (u16)gGameState->unk874 >> 2;
        i = 0;
        if (i < p->participantCount) {
            do {
                p->selectionBits[i + p->selectionPage * 4] = 0;
                i++;
            } while (i < p->participantCount);
        }
    }
    if (gLinkSession && gLinkSession->localPlayer == 0 && p->receiveBit == 5)
        *value |= 8;
    *value |= ReceiveBit() << 2;
    p->receiveBit++;
    if (p->receiveBit > 5)
        p->receiveBit = 0;
}
