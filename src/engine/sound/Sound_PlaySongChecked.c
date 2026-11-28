#include "global.h"
#include "engine/engine_pause.h"
void Sound_PlaySong(u32);

void Sound_PlaySongChecked(u32 song)
{
    if (!Engine_PauseBits(0x400))
        Sound_PlaySong(song);
}
