#include "global.h"
#define SelectionMenu_AdvanceChoice sub_0801A9B4
typedef struct {u32 pad[2],flags;u8 gap[20];s16 x,y;u8 tail[60];} Graphic;
void sub_08019F18(void *,void (*)(void));
void sub_0801ADC0(void);void sub_0801B004(void);
void sub_0801B600(int,int,void *);
void SelectionMenu_AdvanceChoice(u8 *p) {
 Graphic *g=(Graphic *)(p+0x1f4);
 if(!p[0x1e])g->x=64;else g->x=104;
 g->y=40;
 if(++p[0x1e]<=1)sub_08019F18(p,sub_0801ADC0);
 else {sub_0801B600(p[0x21],p[0x22],p+0xd94);sub_08019F18(p,sub_0801B004);}
}
