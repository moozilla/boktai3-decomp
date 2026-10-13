#include "global.h"
struct Panel {u8 pad0[0x3c];u8 counts[4];};
struct Panel*sub_08037DC0(void);void sub_0803A514(void*,int,int);void sub_080397D4(int,int);void sub_0822B2F8(int);
void sub_080395D4(void*p,int index){struct Panel*q=sub_08037DC0();if(q){u8*counts=q->counts;if(counts[index]<=4){int total=0,i=0;do{total+=counts[i];}while(++i<=3);if(total<=9){sub_0803A514(p,index,1);counts[index]++;sub_080397D4(2,1);sub_0822B2F8(885);}}}}
