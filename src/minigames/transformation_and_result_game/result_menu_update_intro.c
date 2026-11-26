#include "minigames/result_game_tutorial/result_tutorial.h"
#include "player/player.h"

s32 ResultMenu_IsPhaseStart(struct ResultMenu *);
u8 ResultMenu_ConfigureLayers(u32, u32);
void ResultGame_InitializeDisplay(struct ResultMenu *);
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void PlayerCue_BeginModeSeven(void *, u32);
void ResultMenu_SetPhase(struct ResultMenu *, void (*)(void));
void ResultMenu_Countdown(void);

void ResultMenu_UpdateIntro(struct ResultMenu *p)
{
    if (ResultMenu_IsPhaseStart(p)) {
        p->layer = ResultMenu_ConfigureLayers(0xe, 0);
        ResultGame_InitializeDisplay(p);
        ScreenTransition_SendCommand(0, 5, 4, 4, 4, 0xFFFF, 0);
        PlayerCue_BeginModeSeven(gPlayers[0], 0);
    }
    if (p->phaseFrame > 0x1f)
        ResultMenu_SetPhase(p, ResultMenu_Countdown);
    else
        p->phaseFrame++;
}
