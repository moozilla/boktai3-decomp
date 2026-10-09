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
void sub_0822B114(u32 speed)
{
 u32 song=gUnk_03005470[12];
 if(song && gSongTable[song].ms==12) {
  struct MusicPlayerInfo *player=gMPlayTable[12].info;
  sub_082300DC(player);
  m4aMPlayVolumeControl(player,0xffff,1);
  m4aSongNumStop(song);
  m4aMPlayFadeIn(player,speed);
 }
}
