#include "global.h"

void Registry1745_SetState(u8 *, u32, void (*)(void));
void LinkSelection_ExchangeParticipantInput(void);

void Registry1745_OnStageIconComplete(u8 *p)
{
    Registry1745_SetState(p, 0x11, LinkSelection_ExchangeParticipantInput);
}
