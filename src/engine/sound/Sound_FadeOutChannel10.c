#include "global.h"
#include "gba/m4a_internal.h"
#include "engine/ram.h"
void m4aMPlayImmInit(struct MusicPlayerInfo *);
void m4aMPlayFadeOut(struct MusicPlayerInfo *, u16);

void Sound_FadeOutChannel10(u32 speed)
{
    if (gMusicSlotSongs[10]) {
        u32 song = gMusicSlotSongs[10];
        u32 index = gSongTable[song].ms;
        struct MusicPlayerInfo *player = gMPlayTable[index].info;
        m4aMPlayImmInit(player);
        m4aMPlayVolumeControl(player, 0xffff, 255);
        m4aMPlayFadeOut(player, speed);
        gMusicSlotSongs[index] = 0;
    }
}
