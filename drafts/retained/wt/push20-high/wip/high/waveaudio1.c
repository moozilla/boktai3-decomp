#include "global.h"
#include "gba/m4a_internal.h"
extern u16 gUnk_03005470[];
extern const struct MusicPlayer gUnk_082643EC[];
void sub_082300DC(struct MusicPlayerInfo*);
void sub_0822B1D8(u32 speed)
{
 u16 *active=gUnk_03005470;
 if(active[12]) {
  u32 index=gSongTable[active[12]].ms;
  struct MusicPlayerInfo *player=gUnk_082643EC[index].info;
  sub_082300DC(player);
  m4aMPlayVolumeControl(player,0xffff,255);
  m4aMPlayFadeOut(player,(u16)speed);
  active[index]=0;
 }
}
