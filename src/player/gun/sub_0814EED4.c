#include "global.h"

void Sound_StopSong(s32);

void Player_StopActionSongOnExit(void)
{
    Sound_StopSong(0xCC);
}
