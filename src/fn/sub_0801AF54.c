#include "global.h"
#define SelectionMenu_AnimateChoice sub_0801AF54
typedef struct {u32 pad[2],flags;u8 gap[20];u32 position[2];u8 gap28[18];u16 palette;u8 gap3c[12];void *colors;u8 tail[20];} Graphic;
int sub_08019F2C(void *);void sub_082199C8(void *,void *,int);
void *sub_08019E58(int);void sub_082216F8(void *,void *,void *,u32,int);
void sub_0801A9B4(void *);
void SelectionMenu_AnimateChoice(u8 *p) {
 Graphic *source=(Graphic *)(p+0x674+p[0x1d]*0x60);
 Graphic *dest=(Graphic *)(p+0xc74+p[0x1e]*0x60);
 if(sub_08019F2C(p)) {
  sub_082199C8(dest,source,13);dest->flags&=~1;
  source->palette=0x374;source->colors=sub_08019E58(1);
  ((struct {u8 pad[0x21];u8 choices[2];} *)p)->choices[p[0x1e]]=p[0x1d];
  *(u32 *)(p+0x34)&=~(1<<p[0x1d]);
  if(!p[0x1e])*(u16 *)(p+0x38)=16;else *(u16 *)(p+0x38)=64;
  *(u16 *)(p+0x3a)=40;
 }
 sub_082216F8(dest->position,source->position,p+0x38,*(u32 *)(p+0x28),3);
 if(*(u32 *)(p+0x28)>7)sub_0801A9B4(p);
 else (*(u32 *)(p+0x28))++;
}
