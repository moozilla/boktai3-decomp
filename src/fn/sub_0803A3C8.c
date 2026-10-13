#include "global.h"
struct Item{u8 state,pad1[0xaf];};struct Owner{u8 pad0[0x5c];struct Item items[16];};
extern void*gUnk_086057DC[];int sub_08036BEC(void);void sub_08249244(void*,void*,int,void*);void sub_0803A36C(void*);
int sub_0803A3C8(struct Owner*p){if(!sub_08036BEC()){struct Item*q=p->items;int i=0;do{sub_08249244(p,q,i,gUnk_086057DC[q->state]);i++;q++;}while(i<=15);sub_0803A36C(p);}return 0;}
