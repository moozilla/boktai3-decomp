#include "global.h"
#include "gba/m4a_internal.h"
extern u16 gUnk_03005470[];
extern const struct MusicPlayer gUnk_082643EC[];
void m4aMPlayFadeOutTemporarily(struct MusicPlayerInfo*,u16);
void sub_0822B180(u32 speed)
{
 u16 *active=gUnk_03005470;
 u32 song=active[12];
 if(song) {
  u32 index=gSongTable[song].ms;
  m4aMPlayFadeOutTemporarily(gUnk_082643EC[index].info,(u16)speed);
  active[index]=0;
 }
}
