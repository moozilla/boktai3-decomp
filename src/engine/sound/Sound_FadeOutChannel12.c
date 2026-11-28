#include "global.h"
#include "gba/m4a_internal.h"
#include "engine/ram.h"
void m4aMPlayImmInit(struct MusicPlayerInfo *);

void Sound_FadeOutChannel12(u32 speed)
{
    u32 song = gMusicSlotSongs[12];
    if (song) {
        u32 index = gSongTable[song].ms;
        struct MusicPlayerInfo *player = gMPlayTable[index].info;
        m4aMPlayImmInit(player);
        m4aMPlayVolumeControl(player, 0xffff, 255);
        m4aMPlayFadeOut(player, (u16)speed);
        gMusicSlotSongs[index] = 0;
    }
}
