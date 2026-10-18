#include "global.h"

void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void ChallengeMenu_SetState(u8 *, void (*)(void));
void ChallengeMenu_UpdateFadeOut(void);

void ChallengeMenu_StartFadeOut(u8 *p)
{
    ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0xFFFF, 0);
    ChallengeMenu_SetState(p, ChallengeMenu_UpdateFadeOut);
}
