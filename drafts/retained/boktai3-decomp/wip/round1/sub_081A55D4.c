#include "global.h"
void sub_08221110(void *,void *,void *,int,int);
void sub_081A55D4(u8 *a,u8 *b,u8 *c,int d,int e,int n) {
 if(n>0) { u8 *z=c,*y=b; int count=n; u8 *x=a; do { sub_08221110(x,y,z,d,e); z+=0x20; y+=0x20; x+=0x20; } while(--count); }
}
