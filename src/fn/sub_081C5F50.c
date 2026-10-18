#include "global.h"

void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void ChallengeResult_SetState(u8 *, void (*)(void));
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_HideImmediately(void);
void TimeResult_UpdateFadeOut(void);

void TimeResult_BeginFadeOut(u8 *p)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_HideImmediately();
    ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0xFFFF, 0);
    ChallengeResult_SetState(p, TimeResult_UpdateFadeOut);
}
