#include "global.h"

void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void ChallengeMenu_SetState(u8 *, void (*)(void));
void ChallengeMenu_UpdateFadeIn(void);

void ChallengeMenu_StartFadeIn(u8 *p)
{
    ScreenTransition_SendCommand(0, 5, 4, 4, 4, 0xFFFF, 0);
    ChallengeMenu_SetState(p, ChallengeMenu_UpdateFadeIn);
}
