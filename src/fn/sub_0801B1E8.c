#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define SelectionMenu_Hide sub_0801B1E8
typedef struct {u32 pad[2],flags;u8 tail[84];} Graphic;
extern u32 gUnk_0300523C;
void SelectionMenu_Hide(u8 *arg) {
 register u8 *p asm("r3")=arg;
 int i,mask;u32 *flags;u32 *state;register Graphic *g asm("r2");
 g=(Graphic *)(p+0xd4);asm("" : "+r"(g));g->flags|=1;
 g=(Graphic *)(p+0x134);asm("" : "+r"(g));g->flags|=1;
 state=&gUnk_0300523C;asm("" : "+r"(state));
 mask=1;asm("" : "+r"(mask));flags=(u32 *)(p+0x67c);i=18;
 do {*flags|=mask;flags+=24;i--;}while(i>=0);
 mask=1;asm("" : "+r"(mask));flags=(u32 *)(p+0x19c);i=2;
 do {*flags|=mask;flags+=24;i--;}while(i>=0);
 mask=1;asm("" : "+r"(mask));flags=(u32 *)(p+0x2bc);i=7;
 do {*flags|=mask;flags+=24;i--;}while(i>=0);
 g=(Graphic *)(p+0x5b4);asm("" : "+r"(g));g->flags|=1;
 {int clear=~4;asm("" : "+r"(clear));*state&=clear;}
}
