#include "global.h"
s32 sub_0822E104(u8 *);void sub_0822E138(u8 *,u8 *);
void sub_0822E18C(u8 *p) {
 s32 count=0,gaps=0,i,j;
 for(i=0;i<16;i++) {
 u8 *q=p+i;
 if(sub_0822E104(q)<0)gaps++;
 else {count++;if(gaps>0)sub_0822E138(q,q-gaps);}
 }
 if(count>1) {
 for(i=0;i<count-1;i++)for(j=i;j<count;j++) {
 if(sub_0822E104(p+i)>sub_0822E104(p+j))sub_0822E138(p+i,p+j);
 }
 }
}
