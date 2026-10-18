#include "global.h"
void Sound_PlaySong(u32);
void Character_OnActionSoundCue(void)
{
    Sound_PlaySong(0x94);
}
