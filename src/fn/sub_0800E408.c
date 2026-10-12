#include "global.h"
// MATCHING_CONCESSION: register-pinning
extern struct Background {u8 pad[24];s16 width;} gUnk_03004C30;
s16 *sub_0800E3E0(int,int,int);int Div(int,int);
#define NumericDisplay_DrawPair sub_0800E408
void NumericDisplay_DrawPair(void *unused,int left,int right){
 int digits[2];s16 *dst;int row,col;register int *values asm("r12");
 digits[0]=left>9;digits[1]=left-digits[0]*10;
 dst=sub_0800E3E0(0,6,11);
 row=0;values=digits;asm("" : "+r"(values));
 for(;row<2;row++){
  int shift=row<<4;int *digit=values;s16 *out=dst;
  for(col=0;col<2;col++){int tile=*digit++ +32;*out++=shift+tile | -12288;}
  dst+=gUnk_03004C30.width*2;
 }

 digits[0]=Div(right,10);digits[1]=right-digits[0]*10;
 dst=sub_0800E3E0(0,10,11);
 row=0;values=digits;asm("" : "+r"(values));
 for(;row<2;){
  int next=row+1;row<<=4;
  {
  int *digit=values;s16 *out=dst;
  for(col=0;col<2;col++){int tile=*digit++ +32;*out++=row+tile | -12288;}
  dst+=gUnk_03004C30.width*2;
  }row=next;
 }

}
