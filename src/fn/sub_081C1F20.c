#include "global.h"
#include "player.h"
#include "game_state.h"

struct S {
    u8 pad0[0x588];
    u16 v;
    u8 pad58A[0x590 - 0x58A];
    u16 multiplier;
};

void Camera_FollowPlayerWithoutCompletion(u32);
void ControlPanel_ClearPlayerInputHistory(void);
void ControlPanel_HideInteractionDisplay(struct S *p);
void ControlPanel_SetUpdate(struct S *p, void (*v)(void));
void Sound_SetChannel10Tempo(u32 t);
void StageIconDisplay_Create(u32, u32, void (*)(void), struct S *);
void SegmentDisplay_CallSelectionScript(void);
void sub_081C1DCC(void);
void ControlPanel_UpdateIdle(void);

void ControlPanel_BeginIdle(struct S *p, u32 flag)
{
    gGameState->menuRewardBalance = 0;
    ControlPanel_ClearPlayerInputHistory();

    p->multiplier = 1;
    p->v = 0;

    Sound_SetChannel10Tempo(0x100);
    Camera_FollowPlayerWithoutCompletion(8);
    ControlPanel_HideInteractionDisplay(p);

    gPlayers[0]->kind = 2;
    *((u8 *)gPlayers[0] + 0x457) = 0;
    *((u8 *)gPlayers[0] + 0x457 + 1) = 0;

    if (flag == 0) {
        StageIconDisplay_Create(1, 0, SegmentDisplay_CallSelectionScript, p);
    } else {
        StageIconDisplay_Create(1, 0, sub_081C1DCC, p);
    }

    ControlPanel_SetUpdate(p, ControlPanel_UpdateIdle);
}
