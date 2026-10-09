#include "global.h"
struct Entry {u32 src;u16 size,count;};
struct Dma {vu32 src,dest,control;};
extern struct Entry gUnk_02039000[];
extern u32 gUnk_03004304;
void ObjGfx_LoadTiles(u32 start)
{
 struct Entry *entry=gUnk_02039000;
 u32 dst=0x06010000+(start<<5);
 u32 i=0,total=gUnk_03004304;
 while(i<total) {
  u32 src=entry->src;
  s32 j=0;
  struct Entry *next=entry+1;
  i++;
  while(j<entry->count) {
   struct Dma *dma=(struct Dma*)0x040000d4;
   dma->src=src;dma->dest=dst;dma->control=(entry->size>>2)|0x84000000;(void)dma->control;
   src+=0x200;dst+=entry->size;j++;
  }
  entry=next;
 }
 gUnk_03004304=0;
}
