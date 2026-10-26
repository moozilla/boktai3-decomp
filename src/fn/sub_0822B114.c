#include "global.h"
#include "gba/m4a_internal.h"
#include "ram.h"
void m4aSongNumStartOrContinue(u16);
void m4aSongNumStart(u16);
void m4aSongNumStop(u16);
void m4aMPlayImmInit(struct MusicPlayerInfo *);
void m4aMPlayFadeIn(struct MusicPlayerInfo *, u16);
void m4aMPlayFadeOut(struct MusicPlayerInfo *, u16);
void m4aMPlayFadeOutTemporarily(struct MusicPlayerInfo *, u16);
void Sound_FadeInChannel12(u32 speed)
{
    u32 song = gMusicSlotSongs[12];
    if (song && gSongTable[song].ms == 12) {
        struct MusicPlayerInfo *player = gMPlayTable[12].info;
        m4aMPlayImmInit(player);
        m4aMPlayVolumeControl(player, 0xffff, 1);
        m4aSongNumStop(song);
        m4aMPlayFadeIn(player, speed);
    }
}
