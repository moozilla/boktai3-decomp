#include "global.h"
#define SelectionMenu_Back sub_0801A920
typedef struct {u32 pad[2],flags;u8 gap[20];s16 x,y;u8 tail[60];} Graphic;
void sub_08019F18(void *,void (*)(void));
void sub_0801ADC0(void);void sub_0801B188(void);
void sub_0822B2F8(int);
void sub_0821980C(void *,void *,int,int);
void sub_08030F20(int,int,int,int);
void sub_08019F44(void *,int);
void SelectionMenu_Back(u8 *p) {
 Graphic *g=(Graphic *)(p+0x1f4);
 if(p[0x1e]) {
  sub_0822B2F8(0xde);
  sub_0821980C(g,p+0x4c,0x4d,0);
  p[0x1e]--;
  ((Graphic *)(p+0xc74+p[0x1e]*0x60))->flags|=1;
  if(!p[0x1e])g->x=16;else g->x=64;
  g->y=40;
  sub_08030F20(2,16,30,2);
  sub_08019F44(p,0);
  sub_08019F18(p,sub_0801ADC0);
 }else {sub_0822B2F8(0xde);sub_08019F18(p,sub_0801B188);}
}
