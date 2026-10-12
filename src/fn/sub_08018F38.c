#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Circle{u8 pad[24];s16 radius;u16 direction,step;u8 dispose,pad1f[3];u16 bank;u16 rows[2][160];s16 x,y;};u16 Sqrt(u32);
#define CircleWindow_GenerateScanlines sub_08018F38
void CircleWindow_GenerateScanlines(struct Circle*p,int bank){int radius=p->radius>>4;register int savedRadius asm("r3")=radius;int square;int top,bottom,i;u16*out;asm("" : "+r"(savedRadius));square=radius*radius;top=p->y-radius;if(top<0)top=0;bottom=p->y+radius;if(bottom>160)bottom=160;{register int relativeTop asm("r0")=savedRadius-p->y;asm("" : : "r"(relativeTop));}out=p->rows[bank];for(i=0;i<top;i++,out++)*out=0;for(i=top;i<bottom;i++,out++){int dy=p->y-i;int width=Sqrt(square-dy*dy);int left=p->x-width,right=p->x+width;if(left<0)left=0;if(right>240)right=240;*out=((left&255)<<8)|(right&255);}for(i=bottom;i<=159;i++,out++)*out=0;p->rows[bank][159]=0;}
