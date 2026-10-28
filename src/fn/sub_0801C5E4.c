#include "global.h"
#include "player.h"
struct P {
    u8 pad[0x3c];
    u32 w3c;
    u8 pad1[0x77];
    u8 bb7;
};
s32 ResultMenu_IsPhaseStart(void);
u8 ResultMenu_ConfigureLayers(u32, u32);
void sub_0801BD44(struct P *);
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void PlayerCue_BeginModeSeven(u32, u32);
void ResultMenu_SetPhase(struct P *, void (*)(void));
void ResultMenu_Countdown(void);
void ResultMenu_UpdateIntro(struct P *p)
{
    if (ResultMenu_IsPhaseStart() != 0) {
        u32 z;
        p->bb7 = ResultMenu_ConfigureLayers(0xe, 0);
        z = 0;
        sub_0801BD44(p);
        ScreenTransition_SendCommand(0, 5, 4, 4, 4, 0xFFFF, z);
        PlayerCue_BeginModeSeven((*(u32 *)gPlayers), 0);
    }
    if (p->w3c > 0x1f) {
        ResultMenu_SetPhase(p, ResultMenu_Countdown);
    } else {
        p->w3c++;
    }
}
