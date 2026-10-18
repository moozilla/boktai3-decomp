#include "global.h"
void Actor_UpdateInteractionSoundDelay(void);
void Actor_InitInteractionSoundDelay(u8 *p)
{
    *(u8 *)(p + 0xF88) = 0xc;
    *(void (**)(void))(p + 0xF98) = Actor_UpdateInteractionSoundDelay;
}
