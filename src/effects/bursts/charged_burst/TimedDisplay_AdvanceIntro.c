#include "effects/bursts/charged_burst.h"

void sub_0823350C(struct ChargedBurst *);
void ChargedBurst_EmitSpark(struct ChargedBurst *);
void ChargedBurst_EmitTerminalDebris(struct ChargedBurst *);
void Sound_PlaySong(u32);

void TimedDisplay_AdvanceIntro(struct ChargedBurst *burst)
{
    sub_0823350C(burst);
    ChargedBurst_EmitSpark(burst);
    if (++burst->timer > 31) {
        burst->core.flags &= ~1;
        Sound_PlaySong(0x133);
        ChargedBurst_SetState(burst, ChargedBurst_EmitTerminalDebris);
    }
}
