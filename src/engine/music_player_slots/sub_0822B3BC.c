#include "engine/shared/global.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
void Sound_PlaySong(u32);
void Sound_PlaySongChecked(u32 n)
{
    u32 m = 0x400;
    if (((gEngineFlags | gScenePauseFlags) & m) == 0)
        Sound_PlaySong(n);
}
