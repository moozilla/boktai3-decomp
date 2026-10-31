#include "player_cue.h"

void PlayerCue_UpdateRunOverlay(struct Player *p)
{
    u16 *parameter = &p->actionEighteenParameter;
    u16 value = 0xBA;
    value <<= 1;
    *parameter = value;
    p->actionEighteenDuration = 0x18;
}
