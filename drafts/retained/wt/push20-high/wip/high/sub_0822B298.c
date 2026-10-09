#include "global.h"
#include "gba/m4a_internal.h"
extern u16 gUnk_03005470[];
extern const struct MusicPlayer gUnk_082643EC[];
u32 Script_SeekToKeyword(u32);s32 Script_GetValue(void);
void sub_0822B298(void)
{
 if(Script_SeekToKeyword('v')) {
  s32 volume=Script_GetValue();
  s32 track;
  if(Script_SeekToKeyword('t')) track=Script_GetValue();else track=255;
  {u16 *active=gUnk_03005470;u32 song=active[10];
  if(song) {
   u32 index=gSongTable[song].ms;
   m4aMPlayVolumeControl(gUnk_082643EC[index].info,(u16)track,(u16)volume);
  }}
 }
}
