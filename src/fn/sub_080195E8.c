#include "global.h"
#define TileAnimation_Advance sub_080195E8
typedef struct {u16 id,time;u8 active,count,frame,frames,unpaused,dirty,remove,pad;u16 source[8],dest[8],duration[8];} Channel;
void TileAnimation_Advance(void *manager,Channel *p) {
 if (++p->time>=p->duration[p->frame]) {
  p->time=0;
  if (++p->frame>=p->frames) p->frame=0;
  p->dirty=1;
 }
}
