#include "global.h"

void LinkSession_SetState(u8 *, u32, void (*)(void));
void LinkSelection_ExchangeParticipantInput(void);

void LinkSession_OnStageIconComplete(u8 *p)
{
    LinkSession_SetState(p, 0x11, LinkSelection_ExchangeParticipantInput);
}
