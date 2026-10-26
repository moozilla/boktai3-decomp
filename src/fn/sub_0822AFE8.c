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
void Sound_PauseChannel10(u32 arg)
{
    u32 speed = arg;
    if (gMusicSlotSongs[10]) {
        u32 song = gMusicSlotSongs[10];
        u32 index = gSongTable[song].ms;
        struct MusicPlayerInfo *player = gMPlayTable[index].info;
        m4aMPlayImmInit(player);
        m4aMPlayVolumeControl(player, 0xffff, 256);
        m4aMPlayFadeOutTemporarily(player, speed);
        gMusicSlotSongs[index] = 0;
    }
}
