#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define ResultMenu_CreateGraphics sub_0801BCA8
void sub_0821983C(void *,void *,int,int,int,int,int,void *);
void *sub_08215184(int);void sub_08217DE0(void *,void *,int);
int ResultMenu_CreateGraphics(u8 *arg) {
 u8 *p=arg;
 struct {u32 zero;u32 position[2];} tmp;register u32 *pos asm("r4")=tmp.position;register u8 *out asm("r4");register int i asm("r5");register u32 *saved asm("r8");register int clear asm("r7");
 asm("" ::: "r7");asm("" : "+r"(pos));tmp.zero=0;
 {register u32 *src asm("r0")=&tmp.zero;register u32 *dst asm("r1");asm("" : "+r"(src));dst=pos;asm("" : "+r"(dst));CpuSet(src,dst,0x05000002);}
 saved=pos;asm("" : "+r"(saved));clear=0;asm("" : "+r"(clear));
 out=p+0x144;i=14;
 do {
  sub_0821983C(out,p+0x104,0xb9,0x11,clear,clear,60,saved);
  out+=0x60;i--;
 }while(i>=0);
 {void *resource=sub_08215184(0x1c1a);
 register u8 *a asm("r8")=p+0xb8;register u8 *b asm("sb")=p+0xb9;
 asm("" : "+r"(a),"+r"(b));
 out=p+0x6e4;i=3;
 do {sub_08217DE0(out,resource,0x11);out+=0x28;i--;}while(i>=0);
 {register int zero asm("r1")=0;asm("" : "+r"(zero));*a=zero;*b=zero;}
 }
}
