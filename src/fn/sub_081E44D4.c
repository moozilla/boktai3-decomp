#include "global.h"

void Sound_PlaySong(u32);

void Actor_BeginInteractionSoundSequence(u8 *p)
{
    Sound_PlaySong(0x11E);
    *(u32 *)(p + 0xF98) = 0x081E44F9;
}
