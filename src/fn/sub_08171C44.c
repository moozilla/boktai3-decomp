#include "global.h"
void sub_0822B2F8(s32);void sub_08165C08(void *,void *,s32);void sub_08166714(void *,s32);void sub_0822E138(s32,s32);void sub_081713A4(void *,s32);
void sub_08171C44(u8 *p) {
 u16 *timer=(u16 *)(p+0xa4e);
 if(!*timer)sub_0822B2F8(0x38a);
 (*timer)++;
 if(*timer<=11) {
 s32 n=12-*timer;
 sub_08165C08(p+0x2ac0,p+0x442c,n);
 sub_08165C08(p+0x2b20,p+0x4434,n);
 sub_08166714(p,n);
 }else {
 u32 *q=(u32 *)(p+0x2aa8);u32 v=*q,m=1;*q=v|m;
 *(u32 *)(p+0x2b08)|=m;*(u32 *)(p+0x41e8)|=m;*(u32 *)(p+0x4248)|=m;
 sub_0822E138(p[0x438a],p[0x4387]);sub_081713A4(p,0);
 }
}
