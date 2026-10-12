// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define ResultMenu_UpdateMarkers sub_0801C0C8
typedef struct {u32 pad[2],flags;u8 pad2[20];s16 x;u8 tail[62];} Graphic;
typedef struct {u16 state,time;} State;
void ResultMenu_UpdateMarkers(u8 *p) {
 State *s=(State *)(p+0x78c); Graphic *g=(Graphic *)(p+0x384); int i; int zero; {register int initial asm("r6")=0; asm("" : "+r"(initial));zero=initial;}
 for(i=4;i>=0;i--,s++,g++) {
 switch(s->state) {
 case 0:break;
 case 1:g->x=(8-s->time)*8+4;if(s->time>7){g->x=4;s->state=2;s->time=0;}break;
 case 2:if(s->time>63){s->state=3;s->time=zero;g->flags|=0x200;}break;
 case 3:if(s->time>7){s->state=zero;s->time=zero;g->flags=(g->flags&~0x200)|1;}break;
 }
 s->time++;
 }
}
