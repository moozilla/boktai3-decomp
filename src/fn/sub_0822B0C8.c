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
void sub_0822B0C8(u32 song)
{
 const struct Song *songs=gSongTable;
 const struct Song *entry=&songs[song];
 if(entry->ms==12) {
  u32 index;
  const struct MusicPlayer *players;
  m4aSongNumStartOrContinue(song);
  players=gMPlayTable;
  index=entry->ms;
  m4aMPlayVolumeControl(players[index].info,0xffff,256);
  gUnk_03005470[index]=song;
 }
}
