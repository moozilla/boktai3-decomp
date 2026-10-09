#include "global.h"
#include "gba/m4a_internal.h"
extern u16 gUnk_03005470[];
void m4aSongNumStartOrContinue(u16);
void m4aSongNumStart(u16);
void m4aSongNumStop(u16);
void sub_082300DC(struct MusicPlayerInfo *);
void m4aMPlayFadeIn(struct MusicPlayerInfo *,u16);
void m4aMPlayFadeOut(struct MusicPlayerInfo *,u16);
void m4aMPlayFadeOutTemporarily(struct MusicPlayerInfo *,u16);
void sub_0822AE7C(u32 song)
{
 u32 index;
 if(gSongTable[song].ms==10) m4aSongNumStartOrContinue(song);
 else m4aSongNumStart(song);
 {const struct MusicPlayer *players=gMPlayTable;
 index=gSongTable[song].ms;
 m4aMPlayVolumeControl(players[index].info,0xffff,256);
 gUnk_03005470[index]=song;}
}
