#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define SelectionMenu_ActorAvailability sub_0801AB1C
extern u8 *gUnk_02000710;
extern u8 gUnk_08E6092C[][32];
u8 *sub_0822D6C8(int);
void SelectionMenu_ActorAvailability(u8 *p) {
 int i,mask; s16 *slot;
 *(u32 *)(p+0x34)=0xffff;
 {u8 *base=gUnk_02000710;asm("" : "+r"(base));mask=1;asm("" : "+r"(mask));slot=(s16 *)(base+0x68);}i=7;
 do {if(*slot>=0)*(u32 *)(p+0x34)&=~(mask<<*slot);slot++;i--;}while(i>=0);
 for(i=0;i<16;i++) {
  u8 *actor=sub_0822D6C8(i);
  if(*actor!=255) {
   int id=*actor;u8 *data=gUnk_08E6092C[id];
   if(id!=46 && *(u32 *)(data+24)==0xffffffff && (*(u32 *)(data+28)&0xffffff)==0xffffff)
    *(u32 *)(p+0x34)&=~(1<<i);
  }
 }
}
