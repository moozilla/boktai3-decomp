#include "global.h"
#include "engine.h"
extern u32 gUnk_030053F0;
void Sound_PlaySong(u32);
void Sound_PlaySongChecked(u32 n)
{
    u32 m = 0x400;
    if (((gEngineFlags | gUnk_030053F0) & m) == 0)
        Sound_PlaySong(n);
}
