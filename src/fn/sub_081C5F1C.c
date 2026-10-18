#include "global.h"

void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void ChallengeResult_SetState(u8 *, void (*)(void));
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_HideImmediately(void);
void TimeResult_UpdateFadeIn(void);

void TimeResult_BeginFadeIn(u8 *p)
{
    ScreenTransition_SendCommand(0, 5, 4, 4, 4, 0xFFFF, 0);
    ChallengeResult_SetState(p, TimeResult_UpdateFadeIn);
}
