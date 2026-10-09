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
void sub_0822AF10(u32 song)
{
 if(song) {
  u16 *slots=gUnk_03005470;
  u16 *slot=&slots[gSongTable[song].ms];
  if(song==*slot) {m4aSongNumStop(song);*slot=0;}
 } else {
  u16 *slot=gUnk_03005470;
  if(slot[10]) {m4aSongNumStop(slot[10]);slot[10]=song;}
 }
}
