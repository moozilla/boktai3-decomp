#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Entry {u8 bytes[4],a,b,flag,kind;u32 timer;};
struct Packet{u8 pad,type;u8 pad2[2];s32 word;};
struct P{u8 pad[0x20];u32 flags;u8 pad1[7];u8 sent;u8 pad2[3];u8 mode;u8 scores[4];u8 pad3[8];struct Entry entries[2];struct Packet packet;};
struct P*sub_08037DC0(void);void sub_08037F5C(struct P*);int sub_08036AA8(void);int sub_08036B64(struct Packet*);void sub_0803A618(int,int,int);
void sub_08037DEC(struct Packet*p){register struct P*s asm("r6")=sub_08037DC0();register struct Entry*e asm("r3");int i;register int shift asm("r5");int value;u32 mask;if(!s)return;e=&s->entries[1];switch(p->type){case 0:e->a=p->word;e->b=(u32)p->word>>8;shift=16;value=p->word;i=0;do{e->bytes[i]=(value>>shift)&15;i++;shift+=4;}while(i<=3);sub_08037F5C(s);break;case 1:mask=2;goto flags;case 2:shift=0;{register u32 v asm("r0")=*(volatile u16*)((u8*)p+6);v &=1;e->flag=v;}value=p->word;i=0;do{e->bytes[i]=(value>>shift)&15;i++;shift+=4;}while(i<=3);mask=8;goto flags;case 3:mask=32;goto flags;case 4:e->timer=*(u16*)&p->word;{register u32 v asm("r0")=(*(volatile u16*)((u8*)p+6))&3;u32 z;e->kind=v;z=0;if(v==1)e->b=z;else if(v==2)e->a=z;}mask=128;goto flags;case 5:if(!sub_08036AA8())s->mode=p->word&3;mask=512;flags:s->flags|=mask;break;}}
void sub_08037ECC(struct Packet*p){struct P*s=sub_08037DC0();u32 mask;if(s){switch(p->type){case 1:mask=1;goto flags;case 2:mask=4;goto flags;case 3:mask=16;goto flags;case 4:mask=64;goto flags;case 5:mask=256;flags:s->flags|=mask;break;case 0:default:break;}s->sent=1;}}
void sub_08037F30(void){struct P*s=sub_08037DC0();if(s&&s->packet.type)sub_08036B64(&s->packet);}
void sub_08037F50(void){sub_08037DC0();}
void sub_08037F5C(struct P*s){struct Entry*a=&s->entries[0];register u8*q asm("r5")=s->entries[1].bytes;register int i asm("r6");u32 z;if(a->flag){register u8*d asm("r5");register u8*t asm("r4");i=0;z=0;t=q;d=q-16;do{*d+=*t;sub_0803A618(i,*t,1);*t=z;t++;d++;i++;}while(i<=3);}else{i=0;z=0;do{u8*p=q+i;sub_0803A618(i,*p,0);*p=z;i++;}while(i<=3);}}
