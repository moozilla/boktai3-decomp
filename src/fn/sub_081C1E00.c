#include "global.h"
#include "ram.h"
struct T {
    u8 f[0x30];
    u16 a30;
    u16 a32;
    u16 a34;
};
extern struct T *volatile gPlayers;
void StageIconDisplay_Create(u32, u32, void (*)(void *), u32);
void PlayerCue_Stand(void *, u32);
void ControlPanel_OnStartPromptComplete(void *);
void ControlPanel_BeginInteraction(void)
{
    u32 v = gSegmentDisplay;
    if (v != 0) {
        StageIconDisplay_Create(0, 0, ControlPanel_OnStartPromptComplete, v);
        PlayerCue_Stand(gPlayers, 7);
        gPlayers->a30 = 0x380;
        gPlayers->a32 = 0x100;
        gPlayers->a34 = 0x580;
    }
}
