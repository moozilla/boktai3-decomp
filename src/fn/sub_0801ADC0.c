#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define SelectionMenu_Choose sub_0801ADC0
extern u16 gUnk_03005260[];
int sub_08019F2C(void *);void sub_0801AC50(void *);void sub_0801AC84(void *);
int sub_0801ACB8(void *);void sub_0822B2F8(int);
void sub_08019F44(void *,int);void sub_0801A01C(void *,int);
u8 *sub_0822D6C8(int);int sub_0822E104(int);
void sub_0801A8FC(void *);void sub_0801A920(void *);
void SelectionMenu_Choose(u8 *p) {
 int index;
 switch(p[0x1e]) {

 case 0:
  if(sub_08019F2C(p))sub_0801AC50(p);
  if(gUnk_03005260[0]&4) {
   if(gUnk_03005260[1]&1) {
    if(++p[0x24]>2)p[0x24]=0;
    sub_0822B2F8(0xdc);
   }
  }else {
   index=sub_0801ACB8(p);
   if(index!=p[0x1d]) {
    {register int x asm("r0")=(index&3)*24+144;register int offset asm("r3");offset=0x1b4;asm("" : "+r"(offset));*(u16 *)(p+offset)=x;
    x=(index>>2)*24+32;offset+=2;{register u16 *out asm("r1")=(u16 *)(p+offset);*out=x;}}
    p[0x1d]=index;sub_0822B2F8(0xdc);sub_08019F44(p,0);
   }
   if(gUnk_03005260[1]&1) {
    if(*sub_0822D6C8(p[0x1d])!=255 && ((1<<p[0x1d])&*(u32 *)(p+0x34)))sub_0801A8FC(p);
    else sub_0822B2F8(0x192);
    return;
   }else if(gUnk_03005260[1]&2) {sub_0801A920(p);return;}
  }
  break;

 case 1:
  if(sub_08019F2C(p))sub_0801AC84(p);
  if(gUnk_03005260[0]&4) {
   if(gUnk_03005260[1]&1) {
    if(++p[0x24]>2)p[0x24]=0;
    sub_0822B2F8(0xdc);
   }
  }else {
   index=sub_0801ACB8(p);
   {register int old asm("r3")=p[0x1d];asm("" : "+r"(old));if(index!=old) {
    {register int x asm("r0")=(index&3)*24+144;register int offset asm("r3");offset=0x1b4;asm("" : "+r"(offset));*(u16 *)(p+offset)=x;
    x=(index>>2)*24+32;offset+=2;{register u16 *out asm("r1")=(u16 *)(p+offset);*out=x;}}
    p[0x1d]=index;sub_0822B2F8(0xdc);sub_0801A01C(p,0);
   }}
   if(gUnk_03005260[1]&1) {
    if(sub_0822E104(p[0x1d])>=0) {int selected=p[0x1d];if((1<<selected)&*(u32 *)(p+0x34))sub_0801A8FC(p);else sub_0822B2F8(0x192);}
    else sub_0822B2F8(0x192);
    return;
   }else if(gUnk_03005260[1]&2) {sub_0801A920(p);return;}
  }
  break;
 }
 (*(u32 *)(p+0x28))++;
}
