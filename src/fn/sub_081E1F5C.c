#include "global.h"
struct Sprite {u8 head[8];u32 flags;u8 gap[0x20-12];s16 x,y;u8 tail[0x60-0x24];};
struct Resource {u32 words[8];};
struct Viewer {u8 p0[0x20];struct Sprite background;struct Resource backgroundResource;u8 p1[8];struct Sprite panel;struct Resource panelResource;u8 p2[8];struct Sprite cursor;struct Resource cursorResource;u8 p3[8];struct Sprite label;struct Resource labelResource;u8 p4[8];struct Resource digitsResource;struct Sprite digitsA[3],digitsB[3],labels[3];u8 p5[0x5cc-0x5c0];s32 state,selection,countA,countB,countC,textIndex,valueA,valueB,previousA,previousB,previousC;};
void sub_0821980C(void*,void*,u16,s32);s32 Mod(s32,s32);s32 Div(s32,s32);
extern const s16 gUnk_086136F0[],gUnk_086137AE[],gUnk_08613BC4[];
u32 sub_0822B590(s32);void sub_0822B358(s32);void sub_0822B2F8(s32);void sub_08220D78(void*,void*,u16,s32,s32);
void sub_081E1C10(void*);void sub_081E1C64(void*);void sub_081E1CDC(void*);
// Move to the previous sound in the selected category, wrapping at its ends.
void sub_081E1F5C(struct Viewer*p){switch(p->selection){
case 0:if(--p->textIndex<0)p->textIndex=p->countA-1;sub_081E1C10(p);break;
case 1:if(--p->valueA<0)p->valueA=p->countB-1;sub_081E1C64(p);break;
case 2:if(--p->valueB<0)p->valueB=p->countC-1;sub_081E1CDC(p);break;
}sub_0822B2F8(0x37d);}
