#include "global.h"
typedef struct {u32 pad[2],flags;u8 gap[20];s16 x,y;u8 tail[60];} Graphic;

#define SelectionMenu_PositionInventoryMarkers sub_0801A53C
extern u8 *gUnk_02000710;
void SelectionMenu_PositionInventoryMarkers(u8 *p) {
 int i,index;
 for(i=0;i<8;i++) {
  Graphic *g=(Graphic *)(p+0x2b4+i*0x60);
  if(i<=7)index=*(s16 *)(gUnk_02000710+0x68+i*2);else index=-1;
  if(index>=0) {
   g->x=(index&3)*24+144;
   g->y=(index>>2)*24+32;
   g->flags&=~1;
  }else g->flags|=1;
 }
}
