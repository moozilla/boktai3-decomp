#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define Presentation_SetItemGraphic sub_08019ECC
extern u8 gUnk_08E60F80[][16];
int sub_0822E104(u32);
void sub_0821980C(u8 *,u8 *,u32,u32);
void Presentation_SetItemGraphic(u8 *p,u8 *q,u32 item) {
 int index=sub_0822E104(item);
 if(index>=0) {
  int offset=index*16;u8 *entry;entry=(u8 *)((u32)gUnk_08E60F80+offset);
  {u8 *context=p+0x6c;register int base asm("r0");int resource;base=0x121;asm("" : "+r"(base));resource=base;asm("" : "+r"(resource));{register int tile asm("r3")=entry[0];resource+=tile;}sub_0821980C(q,context,resource,0);}
  *(u32 *)(q+8)&=~1;
 } else *(u32 *)(q+8)|=1;
}
